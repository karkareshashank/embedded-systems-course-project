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


const char max_addr[2] = {0x7F,0xFF};
char addr[2] = {0x00,0x00};

/*
 * 	This function accepts a buffer and page count as the argument.
 *	It then send the request to the in-built device driver for reading
 *	from the eeprom.
 */
int read_EEPROM(void* buf,int count)
{
        int res;
	int fd;
        int i;
	int j = 0;
	int init_j;
	int page_count;
	char* buffer = (char*)buf;
	char* tmp_buf;

	tmp_buf = (char*)malloc(sizeof(char)*64);

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

	page_count = count;

	while(page_count > 0)
	{
	        // Reading a page from the device
        	res = read(fd, tmp_buf ,64);
	        if(res < 0){
			close(fd);
                	return -1;
		}
		
		init_j = j;
		for(i = init_j;i < init_j+64;i++){
			buffer[j++] = tmp_buf[i - init_j];
		}
		
		change_addr(1,0);
		res = write(fd,addr,2);
		if(res < 0){
			close(fd);
			return -1;
		}
		page_count--;

	}

	printf("\n");

	close(fd);
	return res;
}


/*
 *	This function accepts the data and page count as the argument.
 *      It then splits the input data into chunks of 64 bytes .
 * 	It then adds the address in front of the 64 bytes data and send write request
 *	to in-built driver.
 */
int  write_EEPROM(const void* buf, int count)
{
        int res;
        int i;
	int fd;
	int tmp_count;
	int page_count;
	char* buffer = (char*)buf;
        char data[PAGE_SIZE+2];

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

	tmp_count = count;

	while (tmp_count > 0) {

	        // Constructing the data
        	data[0] = addr[0];
        	data[1] = addr[1];

        	for(i = count-tmp_count; i < PAGE_SIZE + 2;i++){
        	        data[i] = buffer[i-2];
		}


        	// writing to the device
	        res = write(fd,data,PAGE_SIZE+2);
        	if(res < 0){
			close(fd);
        	        return -1;
		}
		usleep(5000);

		tmp_count--;
		change_addr(1,0);
	}


	close(fd);
        return res;
}


/*
 * 	This function accepts page number as argument.
 *	It then generate the proper address based on the 
 *	page offset provided.Then we request write call 
 *	on the built-in driver to just write address
 *	to the device.
 */
int seek_EEPROM(int offset)
{
	
	if(offset > PAGE_NUM-1){
		printf("Offset out of bound (0 - 511) \n");
		return -1;
	}

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

	// Address manuplation 
	change_addr(offset,1);


	// Writing the starting address
        res = write(fd,addr,2);
        if(res == -1){
                printf("Error writing to EEPROM: %s \n",strerror(errno));
                close(fd);
                return -1;
        }



	

	close(fd);
	return 0;
}



/*
 *  if flag is 1, then we move the address to the offset page
 *  else , we shift to next page.
 */
void change_addr(int offset,int flag)
{
	int tmp_addr;
	int tmp1;

	if(flag == 1)   /// set to specified page number
	{
		tmp_addr = offset;	
		addr[1] = (tmp_addr & 0x00000003) << 6;
		addr[0] = (tmp_addr & 0x000001fc) >> 2;
	}	
	else		// set to next page number
	{

		if(addr[0] == 0x7F){
			addr[0] = 0x00;
			addr[1] = 0x00;
			return ;
		}

		tmp1 = addr[1];
		tmp_addr = addr[0];
		tmp_addr = (tmp_addr << 8) | ( tmp1);
		tmp_addr += 64;

		addr[1] = (tmp_addr & 0xff) ;
		addr[0] = (tmp_addr & 0x7f00) >> 8;		
	}

}



