# disa
Simple module to disassembly memory using Linux kernel module. This modules is based on [Zydis](https://github.com/zyantific/zydis) and integrated in this module. Also , there is a user space application to demonstrate  the Zydis library on a test function in user space and disassembly of the same function the kernel space. Also is is able to dissemble internals function of the kernel like: printk, kmalloc etc'. This is a good reference how to use [markdown](https://daringfireball.net/projects/markdown/basics).

# Module structure
The module allows 2 interfaces from user space: 
* Using kernel parameters API. 
    This part of the module demonstrate the use of module parameters API to control the module. There are 2 parameters:
    
    1. *size* - the size of memory to disasmble
    2. *func* - name of build in Linux kernel to disassemble 
    
* Using kernel char device API (using misc device):
The purpose of this interface is to make a file behavior for the disa module using */dev/disasm* using file system calls like open,read,ioctl. this interface  it not ready yet, so the disassembly is done when open system call is called (See test1.c)


# How to run
```bash
git clone https://github.com/yairgd/disa.git
cd disa
make 
sudo insmod module/disa.ko
```

# Testing the module
Run test1 and see its results
```bash
sudo ./test1
dmesg |tail -n50
```

## Testing the misc interface
Compare between the output of test1 function that disasemble func1 (see test1.c) on user space and in the kernel space using disa module. Here is the output of test1 in user space:
```bash
this function  named "func1" with param 123
push rbp
mov rbp, rsp
sub rsp, 0x10
mov [rbp-0x04], edi
mov eax, [rbp-0x04]
mov edx, eax
lea rsi, [0x000055EE2B1475D5]
```
and the same disasebly in kernel space:
```bash
[30799.761522] push rbp
[30799.761530] mov rbp, rsp
[30799.761536] sub rsp, 0x10
[30799.761542] mov [rbp-0x04], edi
[30799.761545] mov eax, [rbp-0x04]
[30799.761548] mov edx, eax
[30799.761552] lea rsi, [0x000055F50A1455D5]
```
Both results are identical with gdb:
```bash
  (gdb) x/10i func1
   0x555555563aba <func1>:  	push   %rbp
   0x555555563abb <func1+1>:    mov    %rsp,%rbp
   0x555555563abe <func1+4>:    sub    $0x10,%rsp
   0x555555563ac2 <func1+8>:    mov    %edi,-0x4(%rbp)
   0x555555563ac5 <func1+11>:   mov    -0x4(%rbp),%eax
   0x555555563ac8 <func1+14>:   mov    %eax,%edx
   0x555555563aca <func1+16>:   lea    0x17b04(%rip),%rsi        # 0x55555557b5d5 <__FUNCTION__.3489>  
```
and func1 eauls to :
```bash
p/u (void*)func1
$14 = 93824992295610
```
and the addr parameter also equals to it:
```bash
cat /sys/module/disasm/parameters/addr 
93824992295610
```
## Testing module parameters API interface
Use this command to get list of inernal functions that module is able to disasebmly 
```bash
cat /sys/module/disasm/parameters/func 
```
The result should be 
```bash
list of functions to disassely:
kmalloc,kfree,printk
```
To disasmble functions use this command:
```bash
# select function 
sudo sh -c 'echo printk > /sys/module/disasm/parameters/func'
# size of memory to disassbly+
sudo sh -c 'echo 24 > /sys/module/disasm/parameters/size'
dmesg | tail -n50
```
And the result is
```bash
[31477.477713] disassembly of printk
[31477.477729] push rbp
[31477.477734] mov rbp, rsp
[31477.477737] push r10
[31477.477742] lea rax, [rbp-0x38]
[31477.477745] lea r10, [rbp+0x10]
[31477.477750] sub rsp, 0x48
[31477.477753] mov [rbp-0x30], rsi
```

# References
Here are some reference sources that used to create this module

http://www.embeddedlinux.org.cn/essentiallinuxdevicedrivers/final/ch05lev1sec7.html</br>
http://olegkutkov.me/2018/03/14/simple-linux-character-device-driver/</br>
https://linux-kernel-labs.github.io/master/labs/device_drivers.html</br>
https://gist.github.com/brenns10/65d1ee6bb8419f96d2ae693eb7a66cc0</br>
https://www.kernel.org/doc/htmldocs/kernel-hacking/routines-module-use-counters.html</br>
https://stackoverflow.com/questions/18456155/what-is-the-difference-between-misc-drivers-and-char-drivers</br>
