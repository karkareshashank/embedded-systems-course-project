#ifndef __structure_H__
#define __structure_H__

struct token{
		int 	token_id;
		int 	time_before_write;
		int 	enqueue_time;
		int 	dequeue_time;
		int	time_after_read;
		char* 	string;
	};

#endif
