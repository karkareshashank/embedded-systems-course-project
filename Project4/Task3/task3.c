/*
 * task2.c
 *
 *  Created on: Nov 19, 2013
 *      Author: shashank
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <setjmp.h>


jmp_buf buffer;
float pi = 3.0;
int flag = 1;


void new_calculate_pi()
{
	int 	i 	= 	0;
	float	tmp	= 	1.0;
	pi = 0.0;

	for(i = 1; 1;i++)
	{
		if(i%2 == 0)
			pi = pi - (4.0 / tmp);
		else
			pi = pi + (4.0 /tmp);
	
		tmp = tmp + 2.0;
		if(setjmp(buffer))
			return;
	}


}



void my_handler(int sig)
{
	printf("Handled \n");
	longjmp(buffer,1);
}


int main(int argc,char** argv)
{
	unsigned long int sig;
	int sum;
	int fd1;
	int fd2;
	int ret;
	int oflags,retval;
	fd_set  set;
	pid_t my_pid;
	pthread_t tid;

	struct sigaction* new;
	struct sigaction* tmp;
	tmp = (struct sigaction*)malloc(sizeof(struct sigaction));
	new = (struct sigaction*)malloc(sizeof(struct sigaction));

	tmp->sa_handler = SIG_IGN;

	new->sa_handler = my_handler;
	sigaction(SIGTERM,new,NULL);
	sigaction(SIGIO,tmp,NULL);

	fd2 = open("/dev/input/my_mouse0",O_RDWR);

	my_pid = getpid();
	ret = ioctl(fd2,1,my_pid);

	new_calculate_pi();


	printf("pi = %1.15f\n",pi);

	ret = ioctl(fd2,2,my_pid);

	if(new)
		free(new);

	close(fd2);

	return 0;
}

