Course project done during the Embedded system programming course at Arizona State University
------------------------------------------------------------------------------------------------

Project 1 : A very basic character driver which implements a kernel memory buffer.
	    The buffer is initialized with a default string which containts the uptime of the system.
	    It also implments reader/writers lock so that multiple users can read but only 1 user can 
	    write at a time. Also contains the user-space application to test the driver.

Project 2 : A driver which access the high-resolution timer present on OMAP processor
	    present on Beagleboard Xm. Another driver which implements the ring buffer and can
	    user process can read and write to the device.

Project 3 : Modified I2C driver for EEPROM.

Project 4 : Modified Mouse event driver to send the signal to all the 
	    task registered via ioctl.
