#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include "my_signal_api.h"

typedef struct node{
	struct node* next;
	struct node* prev;
	pthread_t thread_id;
}node_t;

node_t* head = NULL;

static int once_flag = 0;


void my_api_handler();
void init_api();
void send_signal();


void my_api_handler()
{
	printf("handled_in_api \n");
	send_signal();
}

void init_api()
{
	struct sigaction* new;
        new = (struct sigaction*)malloc(sizeof(struct sigaction));

        new->sa_handler   = my_api_handler;
        sigaction(SIGIO,new,NULL);


}

void send_signal()
{
	
	if(once_flag == 0){
		node_t* tmp;
		tmp = head;

		if(tmp == NULL)
			return;
		else{
			do{
				pthread_kill(tmp->thread_id,SIGIO);	
				tmp = tmp->next;
			}while(tmp != head);
		}
		once_flag = 1;
	}

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
	
	if(tmp2)
		free(tmp2);
}



