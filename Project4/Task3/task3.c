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
#include <sys/select.h>


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
	int i;
	int j;
	int fd;

	int all_fd[10];
	int top = 0;
	int ret;

	char*  filename;

	fd_set  set;

	pid_t my_pid;

	struct sigaction* new;
	struct sigaction* tmp;
	
	filename = (char*)malloc(sizeof(char)*25);
	
	tmp = (struct sigaction*)malloc(sizeof(struct sigaction));
	new = (struct sigaction*)malloc(sizeof(struct sigaction));


	tmp->sa_handler = SIG_IGN;
	new->sa_handler = my_handler;
	sigaction(SIGTERM,new,NULL);
	sigaction(SIGIO,tmp,NULL);

	sprintf(filename,"/dev/input/my_mouse");

	for(i = 0 ;i < 100;i++)
	{
		sprintf(filename,"/dev/input/my_mouse%d",i);
		fd = open(filename,O_RDWR);
		if(fd != -1)
			all_fd[top++] = fd;
	}

	FD_ZERO(&set);

	for(i = 0;i < top; i++)
		FD_SET(all_fd[i],&set);
		
	select(all_fd[top-1] + 1,&set,NULL,NULL,NULL);
	
	for(i = 0; i < top;i++)
	{
		if(FD_ISSET(all_fd[i],&set) != 0)
		{
			fd = all_fd[i];
			for(j = 0;j < top;j++)
				if(j != i)
					close(all_fd[j]);
			break;
		}

	}


	my_pid = getpid();
	ret = ioctl(fd,1,my_pid);

	new_calculate_pi();


	printf("pi = %1.15f\n",pi);

	ret = ioctl(fd,2,my_pid);

	if(new)
		free(new);

	close(fd);

	return 0;
}

