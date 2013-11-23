/*
 * task2.c
 *
 *  Created on: Nov 19, 2013
 *      Author: shashank karkare
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <errno.h>
#include <error.h>


float pi = 3.0;


void new_calculate_pi()
{
	sigset_t signal_set;
	int 	sig;
	int 	i 	= 	0;
	float	tmp	= 	1.0;
	pthread_t tid;
	pi = 0.0;
	
	tid = pthread_self();
	sigemptyset(&signal_set);
	sigaddset(&signal_set,SIGALRM);
	sigwait(&signal_set, &sig);	

	if(sig == SIGALRM)
		printf("Signal Handled by thread %lu \n",tid);

	for(i = 1;i < 100 ;i++)
	{
		if(i%2 == 0)
			pi = pi - (4.0 / tmp);
		else
			pi = pi + (4.0 /tmp);
	
		tmp = tmp + 2.0;


	}


}





int main(int argc,char** argv)
{
	sigset_t set;
	unsigned long int sig;
	int sum;
	pthread_t tid1,tid2,tid3,tid4,tid5;

	sigemptyset(&set);
	sigaddset(&set,SIGALRM);
	pthread_sigmask(SIG_BLOCK,&set,NULL);

	pthread_create(&tid1,NULL,(void*)new_calculate_pi,NULL);
	pthread_create(&tid2,NULL,(void*)new_calculate_pi,NULL);
	pthread_create(&tid3,NULL,(void*)new_calculate_pi,NULL);
	pthread_create(&tid4,NULL,(void*)new_calculate_pi,NULL);
	pthread_create(&tid5,NULL,(void*)new_calculate_pi,NULL);

	printf("Thread id's are  %lu     %lu    %lu    %lu    %lu \n",tid1,tid2,tid3,tid4,tid5);


	alarm(1);

	// to wait till any on of the thread is fishished
	while(1){
		if(pthread_kill(tid1, 0) == ESRCH)
			break;
		else
			if(pthread_kill(tid2, 0) == ESRCH)
				break;
			else
				if(pthread_kill(tid3, 0) == ESRCH)
					break;
				else
					if(pthread_kill(tid4, 0) == ESRCH)
						break;
					else
						if(pthread_kill(tid5, 0) == ESRCH)
							break;
	}



	return 0;
}

