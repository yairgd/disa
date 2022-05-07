CC=gcc
ZYDIS-DIR = ./zydis
ZYDIS-REPO = https://github.com/zyantific/zydis.git
CFLAGS=-I. -I./$(ZYDIS-DIR)/include  -I./$(ZYDIS-DIR)/build  -I./$(ZYDIS-DIR)/build/dependencies/zycore  -I./$(ZYDIS-DIR)/build_for_app -I$(ZYDIS-DIR)/build_for_app/dependencies/zycore/  -I./$(ZYDIS-DIR)/dependencies/zycore/include/ -I./$(ZYDIS-DIR)  -g3 -O0
# DEPS = unmaker if depend on heaader files

top_srcdir  = $(shell pwd)
export top_srcdir

all: test1  disasm.ko  run_test1


#%.o: %.c $(DEPS) module/%.c
#	$(CC) -c -o $@ $< $(CFLAGS)

test1: libZydis  test1.o 
	$(CC) -o test1 test1.o ./zydis/build_for_app/libZydis.a
	
disasm.ko:  libZydis-kernel.a
	cd module &&\
	make && \
	cd ..

zydis: 
	@if [ ! -d $(ZYDIS-DIR) ] ;\
	then \
	  echo "zydis does not exist, fetching and copiling"; \
	  echo "if any thing is failed ,then you will have to to mannualy"; \
	  git clone  --recurse-submodules --branch master  $(ZYDIS-REPO); \
          cd $(ZYDIS-DIR) ;\
          git submodule update --init;\
	  git reset --hard de9f60df9e32ec42b5a4c66fd548bae7de93abc8 ;\
	fi

libZydis: zydis
	  cd $(ZYDIS-DIR) ;\
	  if [ -d build_for_app ];then rm build_for_app -rf ; fi;\
	  mkdir build_for_app ;\
	  cd build_for_app ;\
	  cmake  -DCMAKE_C_COMPILER_WORKS=1 -DCMAKE_CXX_COMPILER_WORKS=1  ..;\
	  make ;\
	  cd ..;

libZydis-kernel.a: zydis
	  cd $(ZYDIS-DIR) ;\
          if [ -d build_for_kernel ];then rm build_for_kernel -rf ; fi;\
	  mkdir build_for_kernel ;\
	  cd build_for_kernel;\
	  cmake -DCMAKE_HAVE_THREADS_LIBRARY=1 -DCMAKE_THREAD_LIBS_INIT="-lpthread" -DZYDIS_BUILD_TOOLS=off -DZYDIS_BUILD_EXAMPLES=OFF -DCMAKE_C_COMPILER_WORKS=1 -DCMAKE_CXX_COMPILER_WORKS=1  -D CMAKE_C_FLAGS=" -fno-stack-protector -DNDEBUG -fno-pic -mcmodel=kernel  " .. ;\
	  make ;\
	  cd .. ; \
	  cp ./build_for_kernel/libZydis.a ../module/libZydis-kernel.o
clean:
	make -C module clean
	rm $(ZYDIS-DIR) -rf
	rm test1


run_test1:
	@if [ $(shell id -u) -eq 0 ] ;\
	then \
		insmod module/disa.ko ;./test1 ; rmmod disa;	\
	else \
		echo "Please run as root" ;\
	fi  


.PHONY: zydis module run_test1

