#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>


#include "eeprom.h"

char addr[2] = {0x00,0x00};



int read_EEPROM(char* buf,int count)
{
        int res;
	int fd;
        int i;

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


        // Reading from the device
        res = read(fd, buf ,count);
        if(res < 0){
		close(fd);
                return -1;
	}

	close(fd);
	return res;
}



int  write_EEPROM(char* buf, int count)
{
        int res;
        int i;
	int fd;
        char data[count+2];

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



        // Constructing the data
        data[0] = addr[0];
        data[1] = addr[1];

        for(i = 2; i < count+2;i++)
                data[i] = buf[i-2];


        // writing to the device
        res = write(fd,data,count+2);
        printf("write res = %d\n",res);
        if(res < 0){
		close(fd);
                return -1;
	}



        usleep(5000);

	close(fd);
        return res;
}




int seek_EEPROM(int offset)
{
	if(offset > PAGE_NUM-1)
		return -1;

	int fd;
	int res;

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


	// Writing the data
        res = write(fd,addr,2);
        if(res == -1){
                printf("Error writing to EEPROM: %s \n",strerror(errno));
                close(fd);
                return -1;
        }



	addr[0] = 0x00;
	addr[1] = 0x00;


	close(fd);
	return 0;
}

