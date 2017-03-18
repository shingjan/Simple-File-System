#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "header/sfs.h"
#include "header/command.h"
#include "header/open_t.h"
#include "header/write_t.h"
#include "header/read_t.h"

int main(int argc, char* argv[])
{
    printf("//-----------Simple file system--------------//\n");
	printf("tshell### \n");

    if(getSuperBlock(0)!=INODE_OFFSET){
    	if(argc>1)
	    	init_the_file_system(argv[1]);
	    else{
	    	init_the_file_system(file_name);
	    }
    }

    char command[80];
    char path[80];
    char path1[80];

    while (1) {

        printf("> ");
        scanf("%s", &command);

        if (strcmp(command, "mkfs_t") == 0) { // make file

            //printf("Make File, Path: ");
            scanf("%s", path);

            make_file(path);
        }
        else if (strcmp(command, "mkdir_t") == 0) { // make dir

            //printf("Make Dir, Path: ");
            scanf("%s", path);

            make_dir(path);
        }
        else if (strcmp(command, "exit_t") == 0) {
            printf("Exit\n");
            break;
        }
        else if (strcmp(command, "cd_t") == 0) { // cd

            //printf("CD, Path: ");
            scanf("%s", path);
            cd_path(path);
        }
        else if (strcmp(command, "ls_t") == 0) { // ls
            //printf("LS\n");

            ls_command();
        }
        else if (strcmp(command, "cat_t") == 0) { // cat

            //printf("Cat, Path: ");
            scanf("%s", path);

            cat(path);
        }
        else if (strcmp(command, "external_cp") == 0) { // external

            //printf("Out to In, In Path: ");
            scanf("%s", path);

            //printf("Out to In, Out Path: ");
            scanf("%s", path1);

            external(path1, path);
        }
        else if (strcmp(command, "internal_cp") == 0) { // internal

            //printf("In to Out, In Path: ");
            scanf("%s", path);

            //printf("In to Out, Out Path: ");
            scanf("%s", path1);

            internal(path1, path);
        }
        else if (strcmp(command, "cp_t") == 0) { // internal

            //printf("In to In, Source Path: ");
            scanf("%s", path);
            //printf("In to Out, Destine Path: ");
            scanf("%s", path1);

            inner_copy(path, path1);
        }
        else {
            printf("Invalid Command, please try again\n");
        }

    }

    return 0;
}
