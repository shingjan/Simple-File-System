#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include "sfsheader.h"

int main(int argc, char *argv[])
{
	int db = open (argv[1], O_RDWR, 660);
	if (db == -1)
	{
		print("This should be a HD file.\n");
		return -1;
	}



	//if database is finished establishing
	printf("File system is established!.\n");
	return 0;
}
