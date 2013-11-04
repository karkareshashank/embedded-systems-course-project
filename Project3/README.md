	I2C Driver to Access EEPROM
================================================

In this project , we write :


1)	An user space application to access the EEPROM on I2C Bus using the default i2c-dev.c device driver.
	i2c-dev.c does not have any notion of pages , so we write an wrapper function to handle the pages.

2) 	A Driver  which implements read , write and seek operation. This driver accepts the data in the multiple of page size
	and reads the data in multiple of page size.
	This driver is blocking , so it blocks till the data from the EEPROM is received or write ack is received.

3)	A Driver which is similar to second one but this time its non-blocking .
	It implements work queues. This driver gives work to work queue and return immediatly.
	Its now user application's work to continuousely retry the read/write operation till 0 received(which indicates success).
	
