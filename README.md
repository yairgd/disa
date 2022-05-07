# disa
A simple module to disassembly memory using a Linux kernel module. This module is based on [Zydis](https://github.com/zyantific/zydis) and integrated into this module. Also, there is a userspace application to demonstrate the Zydis library on a test function in user space and disassembly of the same c  function at the kernel space. Also can dissemble internal c functions of the kernel like printk, kmalloc etc'. This document writes with MarkDown, and [here](https://daringfireball.net/projects/markdown/basics) is a useful reference explaining how to use it.

## Module structure
The module allows two interfaces from userspace: 
* Using kernel parameters API:</br>
    This part of the module demonstrates the use of module parameters API to control the module. There is one parameter named *func* and it uses to select from userspace the internal function to disassemble. 

* Using kernel char device API (using a misc device):</br>
    The purpose of this interface is to make a file behavior for the disa module using */dev/disa* using file system calls open, read,ioctl, and its content is the disassembly text code of a selected function. The selected function can be one of two:</br>
    1. Internal kennel function (see kernel parameter *func*)  </br>
    2. The local function of a process and it set using ioctl system call.
    


## Build the module
```bash
git clone https://github.com/yairgd/disa.git
cd disa
make 
```

## Testing the module
Run test1,test2.py are unit tests for this module and to load into the kernel use this command:
```bash
sudo insmod module/disa.ko
sudo ./test1
sudo ./test2.py
```

## Testing of disassembly of userspace function
Compare between the output of test1 function that disassembles func1 (see test1.c) on userspace and in the kernel space using disa module. Here is the output of test1 in userspace:
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
push rbp
mov rbp, rsp
sub rsp, 0x10
mov [rbp-0x04], edi
mov eax, [rbp-0x04]
mov edx, eax
lea rsi, [0x000055F50A1455D5]
```
Both results are identical with gdb:
```bash
  (gdb) x/10i func1
   0x555555563aba <func1>:      push   %rbp
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
## Testing of disasbly internal kernel function
Use this command to get list of inernal functions that module is able to disasebmly. 
```bash
sudo su -c '/sys/module/disasm/parameters/func'
```
Here is a pyhton code to use when its required to disasmble the code of *kfree*:
```python
# select intenal function to disasembly  
f = open("/sys/module/disasm/parameters/func","w");
f.write("kfree");
f.close();
# read the disasmbly data as file
f=open("/dev/disa","r");
a = f.read(256);
a = a.replace(';','\n');
print ( a);
f.close();
```
And the result is:
```bash
push rbp
mov rbp, rsp
push r13
push r12
mov r13, [rbp+0x08]
push rbx
mov rbx, rdi
nop
cmp rbx, 0x10
jbe 0xFFFFFFFF8117C71D
mov r10d, 0x80000000
mov rax, 0x77FF80000000
mov rdi, 0xFFFFEA0000000000
add r10, rbx
cmovb rax, [0xFFFFFFFF81E0D010]
add r10, rax
```
To use the module in bash command line type:
```bash
sudo cat /dev/disa | sed -e $'s/;/\\\n/g'
```


## Supervisor Mode Access Prevention
When the module has to disassemble the function from userspace, it has to copy the function data from the userspace to kernel space to handle the parsing of memory in the kernel space. In the previous driver versions, the kernel had direct access to userspace. However, it ran on an earlier kernel version, and it had worked. In the new versions of the kernel, it must use copy_from_user. Refer [here](https://lwn.net/Articles/517475/) for more info:
>Intel's new "Supervisor Mode Access Prevention" (SMAP) feature changes that situation; those wanting the details can find them starting on page 408 of >this reference manual [PDF]. This extension defines a new SMAP bit in the CR4 control register; when that bit is set, any attempt to access user-space >memory while running in a privileged mode will lead to a page fault. Linux support for this feature has been posted by H. Peter Anvin to generally >positive reviews; it could show up in the mainline as early as 3.7. 


        
## References
Here are some reference sources that used to create this module</br>
[1] [misc device](http://www.embeddedlinux.org.cn/essentiallinuxdevicedrivers/final/ch05lev1sec7.html)    
[2] [char device](http://olegkutkov.me/2018/03/14/simple-linux-character-device-driver/)  
[3] [example of char device](https://gist.github.com/brenns10/65d1ee6bb8419f96d2ae693eb7a66cc0)  
[4] [misc vs char device](https://stackoverflow.com/questions/18456155/what-is-the-difference-between-misc-drivers-and-char-drivers)  
[5] [fix camke pthread problem](https://github.com/alicevision/geogram/issues/2)  
[6] [misc device](https://www.linux.it/~rubini/docs/misc/misc.html)  
[7] [questions about it Supervisor mode access prevention](https://www.linuxquestions.org/questions/linux-kernel-70/doubt-with-kernel-module-4175683252/)
