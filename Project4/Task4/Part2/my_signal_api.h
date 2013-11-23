#ifndef __my_signal_api_H__
#define __my_signal_api_H__

#include <pthread.h>

typedef struct node{
        struct node* next;
        struct node* prev;
        pthread_t thread_id;
}node_t;


void register_thread_for_signal(pthread_t);
void deregister_thread_for_signal(pthread_t);

void my_api_handler();
void init_api();
void wait_for_sigio();



#endif	/* my_signal_api.h */
