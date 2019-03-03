CC=gcc
ZYDIS-DIR = ./zydis
ZYDIS-REPO = https://github.com/zyantific/zydis.git
CFLAGS=-I. -I./$(ZYDIS-DIR)/include  -I./$(ZYDIS-DIR)/build  -I./$(ZYDIS-DIR)/build/dependencies/zycore  -I./$(ZYDIS-DIR)/build_for_app -I$(ZYDIS-DIR)/build_for_app/dependencies/zycore/  -I./$(ZYDIS-DIR)/dependencies/zycore/include/ -I./$(ZYDIS-DIR)  -g3 -O0
# DEPS = unmaker if depend on heaader files

top_srcdir  = $(shell pwd)
export top_srcdir

%.o: %.c $(DEPS) module/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

test1: zydis  disasm.ko test1.o 
	$(CC) -o test1 test1.o ./zydis/build_for_app/libZydis.a
disasm.ko: zydis libZydis-kernel.a
	cd module &&\
	make && \
	cd ..

zydis: 
	@if [ ! -d $(ZYDIS-DIR) ] ;\
	then \
	  echo "zydis does not exist, fetching and copiling"; \
	  echo "if any thing is failed ,then you will have to to mannualy"; \
	  git clone --branch master  $(ZYDIS-REPO); \
          git reset --hard de9f60df9e32ec42b5a4c66fd548bae7de93abc8 ;\
	  cd $(ZYDIS-DIR) ;\
          git submodule update --init;\
	  mkdir build_for_kernel ;\
	  cd build_for_kernel;\
	  cmake -D CMAKE_C_FLAGS="-fno-stack-protector -DNDEBUG -fno-pic -mcmodel=kernel  " .. ;\
	  make ;\
	  cd .. ;\
	  mkdir build_for_app ;\
	  cd build_for_app ;\
	  cmake ..;\
	  make ;\
	  cd ..;\
	fi
libZydis-kernel.a:
	cp $(ZYDIS-DIR)/build_for_kernel/libZydis.a module/libZydis-kernel.a
clean:
	make -C module clean
	rm $(ZYDIS-DIR) -rf
	rm test1


run_test1:
	@if [ "${EUID}" -eq 0 ] ;\
	then \
		insmod module/disa.ko && ./test1 && rmmod disa	\
	else \
		echo "Please run as root" ;\
	fi  

all: test1 module run_test1

.PHONY: zydis module run_test1

