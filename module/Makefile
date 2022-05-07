#C_INCLUDE_PATH=../zydis/include/Zydis
#CPATH=../zydis/include/Zydis
#top_srcdir=/home/yair/disa/
EXTRA_CFLAGS=-I$(top_srcdir)/zydis/include/Zydis  -I$(top_srcdir)/zydis/build_for_kernel/   -I$(top_srcdir)/zydis/dependencies/zycore/include/ -I$(top_srcdir)/zydis/build_for_kernel  -I$(top_srcdir)/zydis/build_for_kernel/ -I$(top_srcdir)/zydis/build_for_kernel/dependencies/zycore/ -I$(top_srcdir)/zydis/include/ -DZYAN_NO_LIBC 

#-Wundef 
#-Wframe-larger-than=4096 -Wint-to-pointer-cast -mcmodel=kernel 
 

obj-m += disasm.o


disasm-objs += disa.o ./libZydis-kernel.o 

#disasm-objs +=  disa.o ../zydis/src/Decoder.o ../zydis/src/FormatterATT.o ../zydis/src/FormatterBuffer.o ../zydis/src/FormatterIntel.o ../zydis/src/MetaInfo.o ../zydis/src/Register.o ../zydis/src/String.o ../zydis/src/DecoderData.o ../zydis/src/FormatterBase.o ../zydis/src/Formatter.o ../zydis/src/Mnemonic.o ../zydis/src/SharedData.o ../zydis/src/Utils.o ../zydis/src/Zydis.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
