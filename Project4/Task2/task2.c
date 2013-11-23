/*
 * task2.c
 *
 *  Created on: Nov 19, 2013
 *      Author: shashank karkare
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
jmp_buf buffer;


/*
 *   Calculates the calue of the pi
 *   using the sum of the infinite series.
 *   Computation stops when u recive SIGALRM signal.
 */
void new_calculate_pi()
{
	int 	i 	= 	0;
	float	tmp	= 	1.0;
	pi = 0.0;

	if(setjmp(buffer) == 0){
	for(i = 1; 1 ;i++)
	{
		if(i%2 == 0)
			pi = pi - (4.0 / tmp);
		else
			pi = pi + (4.0 /tmp);
	
		tmp = tmp + 2.0;

	}
	}


}



void my_handler(int sig)
{
	printf("Stopping the pi calculation \n");
	longjmp(buffer,1);
}

void reg_signal()
{
	struct sigaction* new;

        new = (struct sigaction*)malloc(sizeof(struct sigaction));

        new->sa_handler = my_handler;
        sigaction(SIGALRM,new,NULL);

	if(new)
		free(new);

}


int main(int argc,char** argv)
{
	unsigned long int sig;
	int sum;
	pthread_t tid;

	reg_signal();

	alarm(1);

	new_calculate_pi();

	
	printf("pi = %1.15f\n",pi);


	return 0;
}

