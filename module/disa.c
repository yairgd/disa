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

#define DEVICE_NAME "disa"

MODULE_LICENSE("GPL");     
MODULE_AUTHOR("Yair Gadelov"); 
MODULE_DESCRIPTION("disssmbly module"); 
MODULE_VERSION("0.1");  

#include <Zydis/Zydis.h>

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
	ZyanUSize offset ;
	ZyanU64 runtime_address; 
	unsigned long addr ;
	int size;
};
static struct disa_params disa_params1;
#if 0
	static
void disa (void *p,int n,ZyanUSize offset )
{
	ZyanU8 *data  = p;
	//  int sizeofp = n;

	ZyanU64 runtime_address = (ZyanU64)p;
	const ZyanUSize length = n; 

	ZydisDecoderInit(&disa_params.decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);

	// Initialize formatter. Only required when you actually plan to do instruction
	// formatting ("disassembling"), like we do here
	ZydisFormatterInit(&disa_params.formatter, ZYDIS_FORMATTER_STYLE_INTEL);

	// Loop over the instructions in our buffer.
	// The runtime-address (instruction pointer) is chosen arbitrary here in order to better
	// visualize relative addressing
	while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&disa_params.decoder, data + offset, length - offset,
					&disa_params.instruction)))
	{
		// Print current instruction pointer.
		//printk("%016" PRIX64 "  ", runtime_address);

		// Format & print the binary instruction structure to human readable format	
		ZydisFormatterFormatInstruction(&disa_params.formatter, &disa_params.instruction, disa_params.buffer, sizeof(disa_params.buffer),
				runtime_address);
		printk("%s\n",disa_params.buffer);

		offset += disa_params.instruction.length;
		runtime_address += disa_params.instruction.length;
	}
}
#endif

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
char func_value_buf[256];
static int func_set(const char *val, const struct kernel_param *kp)
{
	const struct _func  *f = funcs;
	if ( param_set_charp(val, kp)<0)
		return -EINVAL;
	while (f->name) {
		if (strncmp (f->name,func_name,strlen (f->name))==0) {
			if (f->fp) {
				printk ("disassembly of %s\n",f->name);

				//disa (f->fp,size,0);
			}
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

/* set user defined address to read from */
static int addr_set(const char *val, const struct kernel_param *kp)
{
	return param_set_ulong(val, kp);
}

static const struct kernel_param_ops addr_ops = {
	.set	= addr_set,
	.get	= param_get_ulong,
};


static unsigned long addr;
module_param_cb(addr, &addr_ops, &addr,  S_IRUGO | S_IWUSR );




/* Close watchdog */
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
//	ZyanUSize offset = disa_params->offset;
	ZyanU8 *data  = (void*)disa_params->addr;
	const ZyanUSize length =  disa_params->size;
//	ZyanU64 runtime_address = disa_params->addr;


	//  int sizeofp = n;



	// Loop over the instructions in our buffer.
	// The runtime-address (instruction pointer) is chosen arbitrary here in order to better
	// visualize relative addressing
	while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&disa_params->decoder, data + disa_params->offset, length - disa_params->offset,
					&disa_params->instruction)))
	{
		// Print current instruction pointer.
		//printk("%016" PRIX64 "  ", runtime_address);

		// Format & print the binary instruction structure to human readable format	
		ZydisFormatterFormatInstruction(&disa_params->formatter, &disa_params->instruction, disa_params->buffer, sizeof(disa_params->buffer),
				disa_params->runtime_address);

		printk("%s\n",disa_params->buffer);

		disa_params->offset += disa_params->instruction.length;
		disa_params->runtime_address += disa_params->instruction.length;
	}

//	copy_to_user(data_to_user,disa_params->buffer,strlen(disa_params->buffer));
	return 0;
}


	static int
disa_open(struct inode *inode, struct file *file)
{
	struct disa_params *disa_params=  kzalloc (sizeof (struct disa_params) , GFP_USER);
	disa_params->addr = addr;
	disa_params->size = size;
	disa_params->runtime_address = addr; 
	ZydisDecoderInit(&disa_params->decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	ZydisFormatterInit(&disa_params->formatter, ZYDIS_FORMATTER_STYLE_INTEL);
	file->private_data = disa_params;

	//	disa ( kmalloc/*(void*)addr*/,24);
//	disa ((void*)addr,size,0);

	return 0;
}


	static int
disa_release(struct inode *inode, struct file *file)
{

	kfree (file->private_data );
	return 0;
}



/* Driver methods */
struct file_operations fops_disa = {
	.owner = THIS_MODULE,
	.open = disa_open,
	.release = disa_close,
	.read = disa_read,
	.release = disa_release
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

