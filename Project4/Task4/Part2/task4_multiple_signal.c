/*
 * task2.c
 *
 *  Created on: Nov 19, 2013
 *      Author: shashank
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include "my_signal_api.h"

float pi = 3.0;
int flag = 1;


void thread_reg_for_signal()
{

	int 	i 	= 	0;
	float	tmp	= 	1.0;
	pthread_t tid;
	pi = 0.0;
	
	tid = pthread_self();

	printf("waiting for SIGIO ... tid = %lu \n",tid);

	wait_for_sigio();

	

	printf("SIGIO recieved by thread %lu \n",tid);


}

void thread_notreg_for_signal()
{
	pthread_t tid;
	int sig;


	tid = pthread_self();

	printf("Do not want to receive SIGIO  ... tid =  %lu \n",tid);

}

void generate_sigio_signal()
{
	sleep(2);
	kill(getpid(),SIGIO);
	printf("\n SIGIO generated \n");
}




int main(int argc,char** argv)
{
	pthread_t tid0,tid1,tid2,tid3,tid4,tid5;
	

	init_api();


	pthread_create(&tid0,NULL,(void*)generate_sigio_signal,NULL);
	
	pthread_create(&tid1,NULL,(void*)thread_reg_for_signal,NULL);
	pthread_create(&tid2,NULL,(void*)thread_reg_for_signal,NULL);
	pthread_create(&tid3,NULL,(void*)thread_reg_for_signal,NULL);
	
	pthread_create(&tid4,NULL,(void*)thread_notreg_for_signal,NULL);
	pthread_create(&tid5,NULL,(void*)thread_notreg_for_signal,NULL);


	register_thread_for_signal(tid1);
	register_thread_for_signal(tid2);
	register_thread_for_signal(tid3);

	//printf(" Threads created	= %lu %lu %lu %lu %lu \n",tid1,tid2,tid3,tid4,tid5);
	//printf(" threads registerd	= %lu %lu %lu \n",tid1,tid2,tid3);



	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);
	pthread_join(tid3,NULL);


	return 0;
}

