# disa
Simple module to disassembly memory using Linux kernel module. This modules is based on [Zydis](https://github.com/zyantific/zydis) and integrated in this module. Also , there is a user space application to demonstrate  the Zydis library on a test function in user space and disassembly of the same function the kernel space. Also the is an option to dissemble internals function of the kernel like: printk, kmalloc etc'.

# Module structure
The module allows 2 interfaces from user space: 
..* Using kernel parameters API. 
..* Using kernel char device API (using misc device)


# References
Here are some reference sources that used to create this module

[1]: http://www.embeddedlinux.org.cn/essentiallinuxdevicedrivers/final/ch05lev1sec7.html
[2]: http://olegkutkov.me/2018/03/14/simple-linux-character-device-driver/
[3]: https://linux-kernel-labs.github.io/master/labs/device_drivers.html
[4]: https://gist.github.com/brenns10/65d1ee6bb8419f96d2ae693eb7a66cc0
[5]: https://www.kernel.org/doc/htmldocs/kernel-hacking/routines-module-use-counters.html
[6]: https://stackoverflow.com/questions/18456155/what-is-the-difference-between-misc-drivers-and-char-drivers
