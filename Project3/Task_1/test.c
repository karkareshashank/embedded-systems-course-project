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

#define FILENAME  "/dev/i2c-1"
#define DEV_ADDR  0x50

char addr[2] = { 0x10,0x00};



int main(int argc,char** argv,char** uenv)
{
        int fd;
        int i;
        int res;
        char* data;
        char recv_data[15];

	data = (char*)malloc(sizeof(char)*15);


        // Opening the device file
        fd = open("/dev/i2c-2",O_RDWR);
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

	sprintf(data,"00Hello world!!");
	data[0] = addr[0];
	data[1] = addr[1];

	res = write(fd,data,15);
	 if(res < 0){
                printf("Error writing to EEPROM: %s \n",strerror(errno));
                close(fd);
                return -1;
        }

	usleep(5000);

	// lseek
	res = write(fd,addr,2);
	 if(res < 0){
                printf("Error writing to EEPROM: %s \n",strerror(errno));
                close(fd);
                return -1;
        }


	// Reading
	res = read(fd,recv_data,13);
	 if(res < 0){
                printf("Error reading to EEPROM: %s \n",strerror(errno));
                close(fd);
                return -1;
        }

	printf("%s\t",recv_data);


	close(fd);
	return 0;
}


