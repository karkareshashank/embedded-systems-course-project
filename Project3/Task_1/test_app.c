#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "eeprom.h"



int main(int argc,char** argv,char** uenv)
{
	int i;
	int res;
	char data[10];
	char* recv_data;

	recv_data = (char*)malloc(sizeof(char)*10);

	
	sprintf(data,"Hello!!");

	// Writing the data
	res = write_EEPROM(data,7);
	if(res == -1){
		printf("Error writing to EEPROM \n");
		return -1;
	}
	

	// Seeking
	res = seek_EEPROM(0);
	if(res == -1){
                printf("Error seeking EEPROM: \n");
                return -1;
        }

	
	// Reading the data
	res = read_EEPROM(recv_data,7);
	if(res == -1){
		printf("Error writing to EEPROM: \n");
                return -1;
        }


	printf("Data Recv = %s \n",recv_data);

	return 0;
}
