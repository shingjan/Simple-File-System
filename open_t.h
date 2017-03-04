#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include "sfs.h"

int open_t(const char *path, int flags)


int open_t(const char *path, int flags){
	char* token;
	char* cmd;
	char tokenList[11][509];
	int depth;
	int inodeNum = 0;
	int fd = open ("HD", O_RDWR, 660);
//--------------------separate the command first------------------------//
	while((token = strsep(&cmd, "/")) != NULL)
	{	
		if(count >= 1)
		{
			if(strcmp(token,"") != 0){
				strcpy(tokenList[depth++], token);
			}
		}
	
	}
//--------------------and then look up for the path---------------------//
	
}
