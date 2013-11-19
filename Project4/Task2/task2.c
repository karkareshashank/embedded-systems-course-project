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

float pi = 3.0;
int flag = 1;


void new_calculate_pi()
{
	int 	i 	= 	0;
	float	tmp	= 	1.0;
	pi = 0.0;

	for(i = 1; 1 <1000000000000000;i++)
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
}


int main(int argc,char** argv)
{
	unsigned long int sig;
	int sum;
	pthread_t tid;

	struct sigaction* new;

	new = (struct sigaction*)malloc(sizeof(struct sigaction));

	new->sa_handler = my_handler;
	sigaction(SIGALRM,new,NULL);

	alarm(1);

	pthread_create(&tid,NULL,(void*)new_calculate_pi,NULL);

	pause();
	

	printf("pi = %1.15f\n",pi);

	if(new)
		free(new);

	return 0;
}

