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
void random_string_gen(char* string,int pages)
{
	int str_len;	
	char rand_char;
	int  coin;
	int i;

	for(i = 0; i < 64*pages -1; i++)
	{
		coin = random_number_gen(0,10);
		if(coin > 5)
			rand_char = (char)random_number_gen(65,90);
		else
			rand_char = (char)random_number_gen(97,122);
		string[i] = rand_char;
	}	
	string[i] = '\0';
}


void print_string_pagewise(char* string,int pages)
{
	int i;
	int j;
	int temp;
	int page_count = 0;
	char* tmp_page = (char*)malloc(sizeof(char)*65);

	while(page_count < pages){
	
		temp = page_count * 64;	
		j = 0;
		for(i = temp; i < temp+64;i++){
			tmp_page[j] = string[i];
			j++;
		}	
		tmp_page[j] = '\0';

		printf("Page %3d %c  %s  \n",page_count+1,':',tmp_page);	
		page_count++;
	}
	
	return;
}



int main(int argc,char** argv,char** uenv)
{
	int i;
	int res;
	int pages = 10;
	char* data;
	char* recv_data;


	// Seeding for the rand function
	srand(time(NULL));

	data	  = (char*)malloc(sizeof(char)*64*pages);
	recv_data = (char*)malloc(sizeof(char)*64*pages);

	
	
	//Creating random string
	random_string_gen(data,pages);

	printf("SENDING DATA : \n");
	print_string_pagewise(data,pages);
	printf("\n -------------------------------------------------- \n");


	// Writing the data
	res = write_EEPROM(data,pages);
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
	res = read_EEPROM(recv_data,pages);
	if(res == -1){
		printf("Error writing to EEPROM: \n");
                return -1;
        }


	printf("RECEIVED DATA : \n");
	print_string_pagewise(data,pages);


	return 0;
}
