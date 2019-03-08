/*
 * =====================================================================================
 *
 *       Filename:  test1.c
 *
 *    Description: This exmaple uses /dev/disa to dump disasembly code of a local function. 
 *    		   It does with kernel module disa (which build on zydis) and in userspace using zydis  library.
 *
 *        Version:  1.0
 *        Created:  02/15/2019 07:30:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yair Gadelov (yg), yair.gadelov@gmail.com
 *        Company:  Israel
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <asm/ioctl.h>
#define DISA_SETADDR  _IOC(_IOC_WRITE, 'k', 1, sizeof(unsigned long))

void func1(int v)
{
	printf ("this function  named \"%s\" with param %d\n",__FUNCTION__,v);
}


#include <stdio.h>
#include <inttypes.h>
#include <Zydis/Zydis.h>


ZydisFormatter formatter;
char buffer[256];

	static
void disa (void *p,int n)
{
	ZyanU8 *data  = p;
	//  int sizeofp = n;

	ZyanU64 runtime_address = (ZyanU64)p;
	ZyanUSize offset = 0;
	const ZyanUSize length = n; //sizeof(data);
	ZydisDecodedInstruction instruction;

	// Initialize decoder context
	ZydisDecoder decoder;
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
		printf("%s",buffer);
		printf("\n");

		offset += instruction.length;
		runtime_address += instruction.length;
	}
}


int main()
{
	long fd;
	char buff[128];
	int i,j;

	func1(123);
	printf ("\n");	
	printf ("get disasembly data for func1 from user space app\n-----------------------------------\n"); 
	disa (func1,24);
	printf ("\n");

	printf ("get disasembly data for func1 from kernel\n-----------------------------------\n"); 


	fd = open ( "/dev/disa",O_RDONLY);
	unsigned long f = (unsigned long )&func1;	
	ioctl (fd,DISA_SETADDR,&f);
	for  ( i=0;i<1;i++) {
		memset (buff,0,sizeof(buff));
		int n=read  ( fd,buff, sizeof(buff));
		for (j = 0;j<n;j++)
			if (buff[j]==';')
				buff[j]='\n';
		printf ("%s",buff);
	}
	close (fd);
	return 0;
}
