#!/usr/bin/python
# example how to disasemble internal funcion of the kenel and 
# get the results in user space

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

