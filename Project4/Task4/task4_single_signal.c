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
#include <pthread.h>
#include <math.h>
#include <sys/types.h>
#include <sys/syscall.h>

#include "my_signal_api.h"

float pi = 3.0;
int flag = 1;


void new_calculate_pi()
{
	int 	i 	= 	0;
	float	tmp	= 	1.0;
	pi = 0.0;

	for(i = 1; ;i++)
	{
		if(i%2 == 0)
			pi = pi - (4.0 / tmp);
		else
			pi = pi + (4.0 /tmp);
	
		tmp = tmp + 2.0;

	}


}



void my_handler(int sig)
{
	
	printf("Handled \n");
	send_signal();
	
}


int main(int argc,char** argv)
{
	unsigned long int sig;
	int sum;
	pthread_t tid1,tid2,tid3,tid4,tid5;

	struct sigaction* new;
	new = (struct sigaction*)malloc(sizeof(struct sigaction));

	new->sa_handler   = my_handler;
	sigaction(SIGIO,new,NULL);

	init_api();

//	alarm(1);

	pthread_create(&tid1,NULL,(void*)new_calculate_pi,NULL);
	pthread_create(&tid2,NULL,(void*)new_calculate_pi,NULL);
	pthread_create(&tid3,NULL,(void*)new_calculate_pi,NULL);
	pthread_create(&tid4,NULL,(void*)new_calculate_pi,NULL);
	pthread_create(&tid5,NULL,(void*)new_calculate_pi,NULL);

	register_thread_for_signal(tid1);
	register_thread_for_signal(tid2);
	register_thread_for_signal(tid3);


//	pause();
	while(1);	

	printf("pi = %1.15f\n",pi);

	if(new)
		free(new);

	return 0;
}

