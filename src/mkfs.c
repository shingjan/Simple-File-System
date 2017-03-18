#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


#include "header/sfs.h"
#include "header/command.h"
#include "header/open_t.h"
#include "header/write_t.h"
#include "header/read_t.h"


int main(int argc, char*argv[])
{
	printf("File system offset reset!.\n");

	if(argc>1)
		init_the_file_system(argv[1]);
	else
		printf("Input value invalid\n");

	return 0;
}
