Mydriver and gmem_tester
============================

The directory contains Mydriver.c file which implemets a character driver.
It is basically a kernel buffer to which we can write and read from.
Mydriver.c implements only 4 basic operation on the /dev/gmem device,
read, write,open and close.

The directory also contains gmem_tester.c file which is a user-level testing application for our driver. You can read and write using this application to the 
/dev/gmem device.


Usage
========

To compile the driver Mydriver.c and the application gmem_tester.c .
bash~$ make
   

To insert the Mydriver.ko module in the kernel
bash~$ sudo insmod Mydriver.ko


To show the content of the gmem device buffer
bash~$ sudo ./gmem_testser show
   

To append the gmem device buffer. 
(NOTE: Leaving the your_string part empty will generate the error )
bash~$ sudo ./gmem_tester write <your_string>

  
To remove the module from the kernel:
bash~$ sudo rmmod Mydriver


To return the directory to proper state
bash~$ make clean