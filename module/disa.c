/*
 * =====================================================================================
 *
 *       Filename:  disa.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/15/2019 07:18:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yair Gadelov (yg), yair.gadelov@gmail.com
 *        Company:  Israel
 *
 * =====================================================================================
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h> 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/suspend.h>
#include <linux/syscalls.h>
#include <linux/reboot.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>         // kmalloc()
#include <asm/ioctl.h>
#include <Zydis/Zydis.h>

#define DEVICE_NAME "disa"

MODULE_LICENSE("GPL");     
MODULE_AUTHOR("Yair Gadelov"); 
MODULE_DESCRIPTION("disssmbly module"); 
MODULE_VERSION("0.1");  



const struct _func {
	char *name;
	void *fp;
} funcs []={
	{.name="kmalloc",.fp=kmalloc},
	{.name="kfree",.fp=kfree},
	{.name="printk",.fp=printk},
	{0,0}
};


struct disa_params {
	ZydisFormatter formatter;
	char buffer[256];
	ZydisDecodedInstruction instruction;
	ZydisDecoder decoder;
	ZyanU64 runtime_address; 
	unsigned long addr ;
};

static unsigned long addr ;
#define DISA_SETADDR  123
//_IOC(_IOC_WRITE, 'k', 1, sizeof(addr))


/* not needed parameters, just for demo */
static char *devname = "disa";
module_param(devname,charp,0660);

/* parmater size, sets the length of the disassembly data */
static int size_set(const char *val, const struct kernel_param *kp)
{
	/* check addr validity */
	//	return -EINVAL;
	return param_set_int(val, kp);
}

static const struct kernel_param_ops size_ops = {
	.set	= size_set,
	.get	= param_get_int,
};

static int size=24;
module_param_cb(size, &size_ops, &size, S_IRUGO | S_IWUSR);



/* parmeter to set the disassembly function */
static char *func_name;
static int func_set(const char *val, const struct kernel_param *kp)
{
	const struct _func  *f = funcs;
	addr = 0;
	if ( param_set_charp(val, kp)<0)
		return -EINVAL;
	while (f->name) {
		if (strncmp (f->name,func_name,strlen (f->name))==0) {
			addr = (unsigned long)(f->fp);
			return 0;	
		}
		f++;
	}
	return -EINVAL;
}

static int func_get(char *buffer,const struct kernel_param *kp)
{
	const struct _func  *f = funcs;

	snprintf (buffer,128,"list of functions to disassely:\n");
	while (f->name) {
		snprintf (buffer + strlen ( buffer) ,128,"%s",f->name);
		f++;
		if (f->name)
			snprintf (buffer + strlen ( buffer) ,128,",");

	}
	return strlen (buffer);
}

static const struct kernel_param_ops func_ops = {
	.set	= func_set,
	.get	= func_get
};
module_param_cb(func, &func_ops, &func_name,  S_IRUGO | S_IWUSR );


	static int
disa_close(struct inode *inode, struct file *file)
{
	return 0;
}

	static ssize_t
disa_read(struct file *file, char __user *data_to_user,
		size_t len, loff_t *ppose)
{
	struct disa_params *disa_params = file->private_data;
	ZyanU8 *data  = (void*)disa_params->addr;
	printk("!!!!!!!!!!!!!!!!!!!!!!! %ld\n",disa_params->addr);
	ZyanUSize length =  len;
	char buf[64];
	int bufferlen;

	memset (buf,0,sizeof(buf));
	bufferlen = 0;
	disa_params->buffer[0] = 0;
	// Loop over the instructions in our buffer.
	// The runtime-address (instruction pointer) is chosen arbitrary here in order to better
	// visualize relative addressing
	while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&disa_params->decoder, data , length ,&disa_params->instruction)))
	{
		// Print current instruction pointer.
		//printk("%016" PRIX64 "  ", runtime_address);

		// Format & print the binary instruction structure to human readable format	
		ZydisFormatterFormatInstruction(&disa_params->formatter, &disa_params->instruction, buf , sizeof(buf), disa_params->runtime_address);

		bufferlen = strlen ( disa_params->buffer) ;
		if ( bufferlen + strlen (buf) +1 < len  && bufferlen + strlen (buf) + 1 <256  ) {
			strcpy (bufferlen  +  disa_params->buffer , buf);//,strlen (buf));
			bufferlen = strlen ( disa_params->buffer) ;
			disa_params->buffer [bufferlen]=';';bufferlen++;
			disa_params->buffer [bufferlen]=0;
			memset (buf,0,sizeof(buf));
			//printk("%s\n",disa_params->buffer);

			//disa_params->offset += disa_params->instruction.length;
			data += disa_params->instruction.length;
			length -= disa_params->instruction.length;
			disa_params->runtime_address += disa_params->instruction.length;
		} else
			break;
	}

	//	if (bufferlen) {
	//		bufferlen--;
	//		disa_params->buffer[bufferlen] =0;
	//	}
	disa_params->addr = (unsigned long)data;

	(void)copy_to_user(data_to_user,disa_params->buffer,  bufferlen  );
	strcpy (disa_params->buffer,buf);
	return bufferlen;
}


	static int
disa_open(struct inode *inode, struct file *file)
{

	struct disa_params *disa_params; 
	if (!addr)
		return -EINVAL;
	disa_params = kzalloc (sizeof (struct disa_params) , GFP_USER);
	disa_params->addr = addr;
	disa_params->runtime_address = addr; 
	ZydisDecoderInit(&disa_params->decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	ZydisFormatterInit(&disa_params->formatter, ZYDIS_FORMATTER_STYLE_INTEL);
	file->private_data = disa_params;
	return 0;
}

	static int
disa_release(struct inode *inode, struct file *file)
{

	kfree (file->private_data );
	return 0;
}


	static long
disa_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned long setaddr;
	struct disa_params *disa_params = file->private_data;	
	switch (cmd) {
		case DISA_SETADDR:
			if (copy_from_user(&setaddr , (unsigned long * )arg, sizeof(unsigned long)))
            	return -EFAULT;
	disa_params->addr = setaddr;
	disa_params->runtime_address = setaddr; 

	printk("XXXXXXXXXXXXXXXXXXXXXXXXXXXX` %ld %ld\n",disa_params->addr, setaddr);

			break;
		default:
			return -ENOTTY;
	};
	return 0;
}

/* Driver methods */
struct file_operations fops_disa = {
	.owner = THIS_MODULE,
	.open = disa_open,
	.release = disa_close,
	.read = disa_read,
	.release = disa_release,
	.unlocked_ioctl = disa_ioctl
};


/* Misc structure */
static struct miscdevice disa_dev = {
	.minor = 130, /* defined as 130 in
			 include/linux/miscdevice.h*/
	.name = DEVICE_NAME,      /* /dev/DEVICE_NAME */
	.fops = &fops_disa  /* disa driver entry points */
};


/* Module Initialization */
	static int __init
disa_init(void)
{
	addr = (unsigned long) funcs[0].fp;
	misc_register(&disa_dev);
	return 0;
}

/* Module Exit */
	static void __exit
disa_exit(void)
{
	misc_deregister(&disa_dev);
}



module_init(disa_init);
module_exit(disa_exit);

