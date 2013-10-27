#ifndef __eeprom_H__
#define __eeprom_H__

#include <stdio.h>
#include <string.h>
#include <linux/i2c-dev.h>

char 	default_addr[2] = {0x00,0x00};
char    user_addr[2];
int     set_user_addr = 0;

ssize_t  read_EEPROM(int fd, char* buf, ssize_t count);
ssize_t write_EEPROM(int fd, char* buf, char* addr ,ssize_t count);
void     seek_EEPROM(int fd, char* addr);



ssize_t read_EEPROM(int fd, char* buf,ssize_t count)
{
	int res;

	// Reading from the device
	res = read(fd, buf ,count);
	if(res < 0)
		return -1;

	return res;
}




ssize_t write_EEPROM(int fd, char* buf, char* addr ,ssize_t count)
{
	int res;
	int i;
	char* data;

	data = (char*)malloc(sizeof(char)* (count+2));
	
	// Constructing the data
	data[0] = addr[0];
	data[1] = addr[1];
		
	for(i = 2; i < count+2;i++)
		data[i] = buf[i-2];


	// writing to the device
	res = write(fd,data,count+2);
	if(res < 0)
		return -1;

	return res;
}




void seek_EEPROM(int fd,char* addr)
{
	int res;

	set_user_addr = 1;
	user_addr[0]  = addr[0];
	user_addr[1]  = addr[1];

	res = write(fd,user_addr,2);

	
}






#endif
