#ifndef __my_signal_api_H__
#define __my_signal_api_H__

#include <pthread.h>

void register_thread_for_signal(pthread_t);
void deregister_thread_for_signal(pthread_t);





#endif	/* my_signal_api.h */
