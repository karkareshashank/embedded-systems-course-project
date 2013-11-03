#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

#define FILENAME "/dev/i2c_flash-2"
#define DEVICE_ADDR 0x52


/* function to generate random number */
int  random_number_gen(int min, int max)
{
	int rand_time;

	do
	{
		rand_time = rand()%(max) + 1;
	}while(rand_time < min || rand_time > max);

	return rand_time;
}


/* function to generate random string */
void random_string_gen(char* string,int pages)
{
	int str_len;	
	char rand_char;
	int i;

	for(i = 0; i < 64*pages -1 ; i++)
	{
		rand_char = (char)random_number_gen(35,122);
		string[i] = rand_char;
	}	
	string[i] = '\0';
}




int main(int argc,char** argv,char** uenv)
{
	int i;
	int j;
	int res;
	int fd;
	char* data;
	char* recv_data;
	char* page_data;
	int pages = 10;
	int page_size = 64;


	// Seeding for the rand function
	srand(time(NULL));

	data	  = (char*)malloc(sizeof(char)*page_size*pages);
	recv_data = (char*)malloc(sizeof(char)*page_size*pages);
	page_data = (char*)malloc(sizeof(char)*page_size+1);


	// Opening the device file
        fd = open(FILENAME,O_RDWR);
        if(fd == -1){
                printf("Error opening %s: %s \n",FILENAME,strerror(errno));
                return -1;
        }	
	
	res = ioctl(fd, I2C_SLAVE,DEVICE_ADDR);
	if(res < 0){
		printf("Error setting the device address \n");
		return -1;
	}

	//Creating random string
	random_string_gen(data,pages);

	printf("send data = %s \n",data);
	// Writing the data
	do{
		res = write(fd ,data,pages);
	}while(res != 0);
	
	

	// Seeking
	res = lseek(fd,0,SEEK_SET);
	if(res == -1){
                printf("Error seeking EEPROM: \n");
                return -1;
        }

	
	// Reading the data
	do{
		res = read(fd,recv_data,pages);
	}while(res!= 0);


	printf("Recv_data = %s \n",recv_data);

	close(fd);
	return 0;
}
