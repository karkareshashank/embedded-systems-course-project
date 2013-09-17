#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>


/* Function to print the usage message */
void gmem_tester_usage()
{
	printf("[usage]: gmem_tester show             : to view th buffer content \n");
        printf("      or gmem_tester write <string>   : to write the string to the buffer \n");
}



// Main function
int main(int argc,char** argv)
{

	int fd;				// file descriptor for device file
	char argument[10];		// Strores the first argument (show / write )
	char *in_string;		// Stores the entire string to write
	int i;				// Loop variable
	ssize_t ret;			// Stores the return value from the read/write  function


	// Check for the argument 
	if(argc == 1 )			// If no argumnet then error	
	{
		printf("Error: Command need atleast 1 argument \n");
		gmem_tester_usage();
		exit(1);
	}


	//Opening the device file to read or write
	fd = open("/dev/gmem",O_RDWR);
	if(fd == -1)	
	{
		printf("Error opening file /dev/gmem : %s \n",strerror(errno));
		exit(1);
	}


	// Allocating space for in_string variable
	in_string = (char*)malloc(sizeof(char)*1000);
	in_string[0] = '\0';


	// Copying the argument in another variable
	strcpy(argument,argv[1]);


	if(strcmp(argument,"show") == 0)			// If the argument is show , read from the buffer and print //
	{
		if(argc > 2)
		{
			printf("Error: No argument needed with show\n");
			gmem_tester_usage();
			goto exit;
		}

		// reads the data from the /dev/gmem file		
		ret = read(fd, in_string , 256);
		if(ret == -1)
		{
			printf("Error : %s \n",strerror(errno));
			goto exit;
		}

		printf("%s \n",in_string);

	}
	else if(strcmp(argument,"write") == 0)			// If the argument is write , write to the buffer with the given string //
	{
		if(argc <=2)
		{
			printf("Error: No string provided\n");
			gmem_tester_usage();
			goto exit;
		}
			
		// collects all the string parts 
		strcpy(in_string,argv[2]);
		if(argc > 3)
		{
			for(i = 3; i < argc;i++)
			{
				strcat(in_string," ");
				strcat(in_string,argv[i]);
			}

		}

		// writes the data to the /dev/gmem file
		ret = write(fd, in_string, strlen(in_string));
		if(ret == -1)
		{
			printf("Error: %s\n",strerror(errno));
			goto exit;
		}

	}
	else							// Else its a wrong argument to the command //
	{
		printf("Error: Invalid argument to the command\n");
		gmem_tester_usage();
		goto exit;
	}
	

   exit:
	free(in_string);
	close(fd);
	return 0;
}
