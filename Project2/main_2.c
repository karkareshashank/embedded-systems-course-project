#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <unistd.h>

#include "structure.h"

int id = 0;


int main()
{
	int fd;
	ssize_t res;
	ssize_t string_len = 0;
	char*   temp_str;
	struct token* new;
	struct token* in;
	int i;
	
	temp_str = (char*)malloc(sizeof(char)*80);
//	new = (struct token*)malloc(sizeof(struct token));
	in  = (struct token*)malloc(sizeof(struct token)+80);


	fd = open("/dev/Squeue1",O_RDWR);
        if(fd == -1)
        {
                printf("Error Opeing file: %s \n",strerror(errno));
                goto end;

        }
	

	for(i = 0; i < 12;i++)
{	printf("Enter a string: ");
	scanf("%s",temp_str);

	new = (struct token*)malloc(sizeof(struct token));
	new->string = (char*)malloc(sizeof(char)*strlen(temp_str));
	strcpy(new->string,temp_str);
	new->token_id = ++id;
	string_len = sizeof(struct token) + strlen(temp_str);
	
	
	res = write(fd,(char*)new,string_len);
	if(res == -1)
	{
		printf("Error: Queue full \n");
		goto end;
	}
}

	res = read(fd,(char*)in,sizeof(struct token) + 80);
	printf("res = %d\n",res);
	if(res == -1)
	{
		printf("Error: Queue empty \n");
		goto end;
	}


	printf("%d --- %s\n",in->token_id,in->string);




     end:
	free(new);
	free(in);
	free(new->string);
	free(temp_str);
	return 0;
}
