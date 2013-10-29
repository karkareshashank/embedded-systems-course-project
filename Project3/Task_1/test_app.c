#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "eeprom.h"



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
void random_string_gen(char* string)
{
	int str_len;	
	char rand_char;
	int i;

	for(i = 0; i < 64*512 -1; i++)
	{
		rand_char = (char)random_number_gen(35,122);
		string[i] = rand_char;
	}	
	string[i] = '\0';
}




int main(int argc,char** argv,char** uenv)
{
	int i;
	int res;
	char* data;
	char* recv_data;


	// Seeding for the rand function
	srand(time(NULL));

	data	  = (char*)malloc(sizeof(char)*64*512);
	recv_data = (char*)malloc(sizeof(char)*64*512);

	
	
	//Creating random string
	random_string_gen(data);

	// Writing the data
	res = write_EEPROM(data,512);
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
	res = read_EEPROM(recv_data,512);
	if(res == -1){
		printf("Error writing to EEPROM: \n");
                return -1;
        }


	printf("Data Recv = %s \n",recv_data);

	return 0;
}
