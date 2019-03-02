/*
 * =====================================================================================
 *
 *       Filename:  test1.c
 *
 *    Description:  
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

void func1(int v)
{
	printf ("this function  named \"%s\" with param %d\n",__FUNCTION__,v);
}


#include <stdio.h>
#include <inttypes.h>
#include <Zydis/Zydis.h>

int disa1 (void *p,int n)
{
    ZyanU8 *data  = p;
    int sizeofp = n;


    // Initialize decoder context
    ZydisDecoder decoder;
    ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);

    // Initialize formatter. Only required when you actually plan to do instruction
    // formatting ("disassembling"), like we do here
    ZydisFormatter formatter;
    ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

    // Loop over the instructions in our buffer.
    // The runtime-address (instruction pointer) is chosen arbitrary here in order to better
    // visualize relative addressing
    ZyanU64 runtime_address = (unsigned long)p;
    ZyanUSize offset = 0;
    const ZyanUSize length = sizeof(data);
    ZydisDecodedInstruction instruction;
    while (ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&decoder, data + offset, length - offset,
        &instruction)))
    {
        // Print current instruction pointer.
        printf("%016" PRIX64 "  ", runtime_address);

        // Format & print the binary instruction structure to human readable format
        char buffer[256];
        ZydisFormatterFormatInstruction(&formatter, &instruction, buffer, sizeof(buffer),
            runtime_address);
        puts(buffer);

        offset += instruction.length;
        runtime_address += instruction.length;
    }
}
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
	long fd,addr,size;
	char buf[16];

	func1(123);
	/* do disasembly from user sapce using dydis library */
	disa (func1,24);

	/* do disassembly from kernel space */
	addr = (long) func1;
	size=64;
	/* write addr */
	fd = open ( "/sys/module/disasm/parameters/addr",O_WRONLY);
	if (fd<0) {
		printf ("module dis is not installed\n");
		exit (-1);
	}
	snprintf (buf,16,"%ld",addr);
	write( fd, buf, 16);
	close (fd);
	addr = atoi (buf);

	/* wrte size to read */
	fd = open ( "/sys/module/disasm/parameters/size",O_WRONLY);
	if (fd<0) {
		printf ("module dis is not installed\n");
		exit (-1);
	}
	snprintf (buf,16,"%ld",size);
	write( fd, buf, 16);
	close (fd);
	size = atoi (buf);

	fd = open ( "/dev/disa",O_WRONLY);
	close (fd);
	return 0;
}
