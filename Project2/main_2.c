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
#include <pthread.h>

#include "structure.h"

#define MIN_STR_LEN		10
#define MAX_STR_LEN		80
#define MIN_SLEEP_TIME		1		//	in msec
#define MAX_SLEEP_TIME		10		//	in msec
#define TOKEN_LIMIT_PER_THREAD 	100		

int id = 0;					// 	Token_id
pthread_mutex_t		id_lock;		// 	Lock for token id


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


/* function to create thread */
pthread_t start_thread(void *func, void *arg)
{
	pthread_t thread_id;
	int rc;

	rc = pthread_create(&thread_id, NULL, func, arg);
	if (rc)
	{
        	printf("ERROR; return code from pthread_create() is %d\n", rc);
        	exit(-1);
        }

	return(thread_id);
}





/* function for sender thread */
void senderfn(int);

/* function for receiver thread */
void receiverfn();





int main(int argc , char** argv)
{

	pthread_t  thread1;
	pthread_t  thread2;
	pthread_t  thread3;
	pthread_t  thread4;
	pthread_t  thread5;
	pthread_t  thread6;
	pthread_t  thread7;
	int dev1	= 1;
	int dev2	= 2;
	
	// Seed for the rand function
	srand(time(NULL));


	// Starting Multithreading //
	
 	// Sender thread for Squeue1
	thread1 = start_thread(senderfn,&dev1);
	thread2 = start_thread(senderfn,&dev1);
	thread3 = start_thread(senderfn,&dev1);

	// Sender thread for Squeue2
	thread4 = start_thread(senderfn,&dev2);
	thread5 = start_thread(senderfn,&dev2);
	thread6 = start_thread(senderfn,&dev2);

	// Reader thread for both the queues
	thread7 = start_thread(receiverfn,NULL);


	pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);
	pthread_join(thread3,NULL);
	pthread_join(thread4,NULL);
	pthread_join(thread5,NULL);
	pthread_join(thread6,NULL);
	pthread_join(thread7,NULL);



	return 0;
}






/*
 * Sender function which will send TOKEN_LIMIT_PER_THREAD
 * tokens to the device who's file descriptor it accepts 
 * as argument.
 */
void senderfn(int file_num )
{

	int i 			= 0;		// loop variable to loop till TOKEN_LIMIT_PER_THREAD
	int fd			= 0;		// File descriptor
	int res			= 0;		// Stroes the return value of the write fn
	int sleep_time		= 0;		// Stores the sleep time for the thread
	ssize_t string_len	= 0;		// Stores the length of the string passing to the write fn
	char* string		= NULL;		// Stores the random generated string
	struct token* new	= NULL;		// Token to be sent
	char   device_name[20];
	char   dev_num[2];

	if(file_num == 1)
		strcpy(dev_num,"1");
	else
		strcpy(dev_num,"2");


	// creating device filename
	strcpy(device_name,"/dev/Squeue");
	strcat(device_name,dev_num);

	//Opening device file
	fd = open(device_name, O_RDWR);
	if(fd == -1)
	{
		printf("Error opening: %s : %s \n",device_name,strerror(errno));
		return;
	}

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

		pthread_mutex_lock (&id_lock);
		new->token_id = id++;
		pthread_mutex_unlock(&id_lock);
		
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

	close(fd);
}



/*
 * Receiver function which reads from the device till both the devices 
 * empty and  taking nap after reading a token from the device.
 */
void receiverfn(void)
{

	int 	fd_1	   = 0;
	int 	fd_2	   = 0;
	int	res1	   = 0;
	int 	flag 	   = 0;
	int 	res2 	   = 0;
	int 	both_empty = 0;
	int	sleep_time = 0;
	ssize_t string_len = 0;
	struct token* in   = NULL;


	fd_1 = open("/dev/Squeue1",O_RDONLY);
	if(fd_1 == -1)
	{
		printf("Error opening :/dev/Squeue1: %s \n",strerror(errno));
		return;
	}

	fd_2 = open("/dev/Squeue2",O_RDONLY);
	if(fd_2 == -1)
	{
		printf("Error opening: /dev/Squeue2: %s \n",strerror(errno));
		return;
	}

	string_len = sizeof(struct token) + MAX_STR_LEN;
	in = (struct token*)malloc(string_len);


	while(!(res1 == -1 && res2 == -1 && both_empty == 50))
	{
		// Reading from Squeue 1
		res1 = read(fd_1,(char*)in,string_len);
		if(res1 == -1)
			flag = 1;
		else
			printf("%d --- %s \n",in->token_id, in->string);

		while(res1 != -1)
		{
			flag = 0;
			res1 = read(fd_1,(char*)in,string_len);	
			if(res1 == -1)
				break;
			printf("%d --- %s \n",in->token_id,in->string);
		}


		// Reading from Squeue 2
		res2 = read(fd_2,(char*)in,string_len);
		if(res2 == -1)
		{
			if(flag == 1)
				both_empty++;
		}
		else	
			printf("%d --- %s \n",in->token_id,in->string);

		while(res2 != -1)
		{
			flag = 0;
			res2 = read(fd_2,(char*)in,string_len);
			if(res2 == -1)
				break;
			printf("%d --- %s \n",in->token_id,in->string);
		}

		sleep_time = random_number_gen(MIN_SLEEP_TIME,MAX_SLEEP_TIME);
		usleep(sleep_time);

	}	
	
	close(fd_1);
	close(fd_2);
}
