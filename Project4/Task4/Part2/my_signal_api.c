#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

#include "my_signal_api.h"

static node_t* 	head;
static int     	pending_count = 0;
static int	count = 0;



void wait_for_sigio() {
                int sig;

		sigset_t* my_api_sigset;
                my_api_sigset = (sigset_t*)malloc(sizeof(sigset_t));

                sigemptyset(my_api_sigset);
                sigaddset(my_api_sigset,SIGUSR1);
                sigwait(my_api_sigset,&sig);

		pending_count--;

		// Unblocking the SIGIO 
		sigemptyset(my_api_sigset);
		sigaddset(my_api_sigset,SIGIO);
		sigwait(my_api_sigset,&sig);
		

		free(my_api_sigset);
		
}

void handling_func(int sig)
{
	
	node_t* tmp;
	tmp = head;
	pending_count  = count;
	
	if(sig == SIGIO){
		if(tmp == NULL){
		}
		else{
			do{
				pthread_kill(tmp->thread_id,SIGUSR1);
                                tmp = tmp->next;
                        }while(tmp != head);

			while(pending_count);
			tmp = head;

			

			do{
				pthread_kill(tmp->thread_id,SIGIO);
				tmp = tmp->next;
			}while(tmp != head);

	
		}			
		
	}
}

void my_api_handler()
{
	node_t* tmp;
	sigset_t signal_set,old_set,new_set;
	struct sigaction new;
	int sig;

	sigemptyset(&new_set);
	sigaddset(&new_set,SIGIO);
	

	pthread_sigmask(SIG_UNBLOCK,&new_set,NULL);

	new.sa_handler = handling_func;
	sigaction(SIGIO,&new,NULL);

	while(1){
		sleep(1);
	}
	
}

void init_api()
{
	pthread_t  tid;
	sigset_t   signal_set;
	
	head = NULL;

	sigemptyset(&signal_set);
	sigaddset(&signal_set,SIGUSR1);
	sigaddset(&signal_set,SIGIO);

	pthread_sigmask(SIG_BLOCK,&signal_set,NULL);

	pthread_create(&tid,NULL,(void*)my_api_handler,NULL);
	
}


void register_thread_for_signal(pthread_t tid)
{
	node_t* new;

	new = (node_t*)malloc(sizeof(node_t));
	new->next = new;
	new->prev = new;
	new->thread_id = tid;

	

	if(head == NULL){
		head = new;
	}
	else{
		new->next = head;
		new->prev = head->prev;
		head->prev->next = new;;
		head->prev = new;
	}
	count++;

}

void deregister_thread_for_signal(pthread_t tid)
{
	node_t* tmp1;
	node_t* tmp2;

	tmp1 = head;
	
	if(head == NULL)
	{
		return;
	}
	else if( head->next == head)
	{
		if(head->thread_id == tid)
			tmp2 = head;
		head = NULL;

	}
	else{
		do{
			if(tmp1->thread_id == tid)
			{
				tmp2 = tmp1;
				break;
			}
			tmp1 = tmp1->next;

		}while(tmp1 != head);
	
		if(tmp2 == head){
			head = head->next;			
		}
		tmp2->prev->next = tmp2->next;
		tmp2->next->prev = tmp2->prev;

	}	
	
	count--;
	if(tmp2)
		free(tmp2);
}



