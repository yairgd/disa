# disa
Simple module to disassembly memory using Linux kernel module. This modules is based on [Zydis](https://github.com/zyantific/zydis) and integrated in this module. Also , there is a user space application to demonstrate  the Zydis library on a test function in user space and disassembly of the same function the kernel space. Also the is an option to dissemble internals function of the kernel like: printk, kmalloc etc'.

# Module structure
The module allows 2 interfaces from user space: 
..* Using kernel parameters API. 
..* Using kernel char device API (using misc device)

# How to run
```bash
git clone https://github.com/yairgd/disa.git
cd disa
make 
sudo insmod module/disa.ko
```

# Testing the misc interface
Run test1 and see its results
```bash
sudo ./test1
dmesg |tail -n50
```
And compare between the output of test1 function that disasemble func1 (see test1.c) on user space and in the kernel space using disa module. Here is the output of test1 in user space:
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
# Testing module params interface


# References
Here are some reference sources that used to create this module

[1]: http://www.embeddedlinux.org.cn/essentiallinuxdevicedrivers/final/ch05lev1sec7.html
[2]: http://olegkutkov.me/2018/03/14/simple-linux-character-device-driver/
[3]: https://linux-kernel-labs.github.io/master/labs/device_drivers.html
[4]: https://gist.github.com/brenns10/65d1ee6bb8419f96d2ae693eb7a66cc0
[5]: https://www.kernel.org/doc/htmldocs/kernel-hacking/routines-module-use-counters.html
[6]: https://stackoverflow.com/questions/18456155/what-is-the-difference-between-misc-drivers-and-char-drivers
