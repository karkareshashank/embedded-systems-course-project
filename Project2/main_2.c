#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <unistd.h>
#include <time.h>

#include "structure.h"

#define MIN_STR_LEN		10
#define MAX_STR_LEN		80
#define MIN_SLEEP_TIME		1		//	in msec
#define MAX_SLEEP_TIME		10		//	in msec
#define TOKEN_LIMIT_PER_THREAD 	100		

int id = 0;

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

	str_len = random_number_gen(MIN_STR_LEN,MAX_STR_LEN-1);
	for(i = 0; i < str_len; i++)
	{
		rand_char = (char)random_number_gen(35,122);
		string[i] = rand_char;
	}	
	string[i] = '\0';
}


/* function for sender thread */
void senderfn(int);

/* function for receiver thread */
void receiverfn(int , int);





int main(int argc , char** argv)
{
	int fd;
	ssize_t res;
	ssize_t string_len = 0;
	char*   temp_str;
	struct token* new;
	struct token* in;
	int i;
	
	char* string;

	srand(time(NULL));
	string = (char*)malloc(sizeof(char)*MAX_STR_LEN);

	random_string_gen(string);
	printf("String = %s\n",string);


	in  = (struct token*)malloc(sizeof(struct token)+80);


	fd = open("/dev/Squeue1",O_RDWR);
        if(fd == -1)
        {
                printf("Error Opeing file: %s \n",strerror(errno));
                goto end1;

        }
	

	for(i = 0; i < 5;i++)
	{	
		new = (struct token*)malloc(sizeof(struct token));
		new->string = (char*)malloc(sizeof(char)*strlen(string));
		strcpy(new->string,string);
		new->token_id = ++id;
		string_len = sizeof(struct token) + strlen(string);
	
	
		res = write(fd,(char*)new,string_len);
		if(res == -1)
		{
			printf("Error: Queue full \n");
			goto end;
		}
	}


	res = read(fd,(char*)in,sizeof(struct token) + 80);
	printf("res = %d\n",res);
	if(res == -1)
	{
		printf("Error: Queue empty \n");
		goto end;
	}


	printf("%d --- %s\n",in->token_id,in->string);




     end:
	free(new);
	free(new->string);
     end1:	
	free(in);
	free(string);


	return 0;
}






/*
 * Sender function which will send TOKEN_LIMIT_PER_THREAD
 * tokens to the device who's file descriptor it accepts 
 * as argument.
 */
void senderfn(int fd)
{
	int i 			= 0;		// loop variable to loop till TOKEN_LIMIT_PER_THREAD
	int sleep_time		= 0;		// Stores the sleep time for the thread
	ssize_t string_len	= 0;		// Stores the length of the string passing to the write fn
	char* string		= NULL;		// Stores the random generated string
	struct token* new	= NULL;		// Token to be sent
	

	// Allocating memory for the variables
	string 		= (char*)malloc(sizeof(char)*MAX_STR_LEN);
	new    		= (struct token*)malloc(sizeof(struct token));
	new->string 	= (char*)malloc(sizeof(char)*MAX_STR_LEN);

	// Calculating the string length
	string_len 	= sizeof(struct token) + MAX_STR_LEN;


	// Looping for the TOKEN_LIMIT_PER_THREAD times before stopping
	for(i = 0;i < TOKEN_LIMIT_PER_THREAD; i++)
	{	
		random_string_gen(string);			// Generating a random string
		strcpy(new->string,string);
		new->token_id = id++;
		
		res = write(fd,(char*)new,string_len);		// Writing the token to the device
		while(res == -1)				// If failed, retry after a nap
		{
			sleep_time = random_number_gen(MIN_SLEEP_TIME,MAX_SLEEP_TIME);
			usleep(sleep_time);
			res = write(fd,(char*)new,string_len);
		}	
	
		// Taking a nap
		sleep_time = random_number_gen(MIN_SLEEP_TIME,MAX_SLEEP_TIME);
		usleep(sleep_time);
		

	}
}



/*
 * Receiver function which reads from the device till both the devices 
 * empty and  taking nap after reading a token from the device.
 */
void receiverfn(int fd_1, int fd_2)
{



}
