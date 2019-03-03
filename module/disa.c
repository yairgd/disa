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


static ZydisFormatter formatter;
static char buffer[256];
static ZydisDecodedInstruction instruction;
static ZydisDecoder decoder;

static
void disa (void *p,int n,ZyanUSize offset )
{
    ZyanU8 *data  = p;
  //  int sizeofp = n;
   
    ZyanU64 runtime_address = (ZyanU64)p;
    const ZyanUSize length = n; 

    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);

    // Initialize formatter. Only required when you actually plan to do instruction
    // formatting ("disassembling"), like we do here
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

    // Loop over the instructions in our buffer.
    // The runtime-address (instruction pointer) is chosen arbitrary here in order to better
    // visualize relative addressing
    while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, data + offset, length - offset,
        &instruction)))
    {
        // Print current instruction pointer.
        //printk("%016" PRIX64 "  ", runtime_address);

        // Format & print the binary instruction structure to human readable format	
        ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer),
            runtime_address);
        printk("%s\n",buffer);
	
        offset += instruction.length;
        runtime_address += instruction.length;
    }
}


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
				
				disa (f->fp,size,0);
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

/* Pet the dog */
static ssize_t
disa_read(struct file *file, char __user *data,
		size_t len, loff_t *ppose)
{
	return 0;
}


/* Open watchdog */
static int
disa_open(struct inode *inode, struct file *file)
{

//	disa ( kmalloc/*(void*)addr*/,24);
	disa ((void*)addr,size,0);

	return 0;
}




/* Driver methods */
struct file_operations fops_disa = {
	.owner = THIS_MODULE,
	.open = disa_open,
	.release = disa_close,
	.read = disa_read,
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

