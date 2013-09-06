/* Demo Application for Lab 1 */
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



int main(void)
{
	int fd1;
	int res;
	char string1[] = "Hello, CSE438/598 class!";


	/* open devices */	
	fd1 = open("/dev/My_device1", O_RDWR);


	if (fd1<0 )
	{
		printf("Can not open device file.\n");		
		return 0;
	}

	res = write(fd1, string1, strlen(string1)+1);
	
	if(res == strlen(string1)+1)
	{
		printf("Can not write to the device file.\n");		
		return 0;
	}
	
	/* close devices */
	close(fd1);

	
	return 0;
}
