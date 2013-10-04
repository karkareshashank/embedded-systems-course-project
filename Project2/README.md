HRT.c , Squeue.c , main_2.c , structure.h , insert.sh , remove.sh
=================================================================
The directory contains  HRT.c , Squeue.c , main_2.c ,structure.h ,insert.sh , remove.sh.

HRT.c  is a driver for high resolution timer. It uses one of the hardware timer present 
in omap processor for the purpose of counting. It implements open,close,read and ioctl functions.
It also exports a read_hr_counter function to allow other modules to read the counter value.

Squeue.c is a driver for 2 devices Squeue1 and Squeue2. It implements a ring buffer of fixed size to hold the tokens.
It implements open, close , read ,write functions. Token is a specific structure defined in Structure.h file.
It contains token_id , 4 timestamps and a string.

main_2.c is user space application to test both Squeue and HRT driver.
It basically creates tokens and write them to the Squeue devices (Sender thread).
It also read the token from the devices and print them (receiver thread).
There are 3 sender threads that writes to Squeue1 device and 3 sender thread writes to Squeue2 device.
There is 1 receiver threads that reads from both the device.

insert.sh is a shell script which inserts the modules in specific order. It also checks for if the current user
is root or not and depending on it adds sudo before the insmod command.

remove.sh is a shell script which removes the modules in specific order. It also checks for if the current user
is root or not and depending on it adds sudo before the rmmod command. 




Compilation 
============
1) Cross-compilation for target machine
----------------------------------------
To compile HRT.c , Squeue.c and main_2.c for target machine use:

bash~$ make ARCH=arm CROSS_COMPILE=arm-angstrom-linux-gnueabi- KDIR=<your_kernel_source_directory>


2) Compilation for host machine
--------------------------------
It is not possible to compile the files for host machine.
You can only compile and run theses program for baegleboard xm or any omap 3730 device.


Usage
=============

1) To insert the modules ( Note: Perform the insertion of modules in given order only) :
	use sudo if you are not the root user else skip the sudo.
	a) bash~$ [sudo] insmod HRT.ko
	b) bash~$ [sudo] insmod Squeue.ko

	OR

	a) sh ./insert.sh


2) To run the user space application :
	use sudo if you are not the root user else skip the sudo.
	a)bash~$ [sudo] ./main_2

3) To remove the modules ( Note: Perform the removing of modules in the given order only) :
	use sudo if you are not the root user else skip the sudo.
	a)bash~$ [sudo] rmmod Squeue
	b)bash~$ [sudo] rmmod HRT

	OR

	a) sh ./remove.sh
	
	
4) To return the directory to original/proper state use :
	a)bash~$ make clean