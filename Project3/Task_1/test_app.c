#include <stdio.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include "eeprom.h"

#define FILENAME  "/dev/i2c-2"
#define DEV_ADDR  0x50

int main(int argc,char** argv,char** uenv)
{
	int fd;
	int res;
	char* data;
	char* recv_data;
	char  addr[2];

	data = (char*)malloc(sizeof(char)*50); 
	recv_data = (char*)malloc(sizeof(char)*50);
	addr[0] = 0x00;
	addr[1] = 0x00;

	// Opening the device file
	fd = open(FILENAME,O_RDWR);
	if(fd == -1){
		printf("Error opening %s: %s \n",FILENAME,strerror(errno));
		return -1;
	}


	// Setting the address of the slave device
	res = ioctl(fd,I2C_SLAVE,DEV_ADDR);
	if(res == -1){
		printf("Error setting slave address: %s \n",strerror(errno));
		close(fd);
		return -1;
	}

	sprintf(data,"Hello World");

	// Writing the data
	res = write_EEPROM(fd,data,addr,strlen(data));
	if(res == -1){
		printf("Error writing to EEPROM: %s \n",strerror(errno));
		close(fd);
		return -1;
	}

	
	seek_EEPROM(fd,addr);
	
	// Reading the data
	res = read_EEPROM(fd,recv_data,strlen(data));
	if(res == -1){
		printf("Error writing to EEPROM: %s \n",strerror(errno));
                close(fd);
                return -1;
        }


	close(fd);	
	return 0;
}
