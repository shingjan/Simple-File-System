#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "sfs.h"
#include "open_t.h"
#include "read_t.h"
#include "write_t.h"

int main()
{
	int current_direct_inode_number = 0;
	struct inode inode = get_inode(0);
	struct inode current_inode;

	char* str = NULL;
	char cmd[10];
	char* token = NULL;
	char input[2][MAX_COMMAND_LENGTH];
	size_t len = MAX_COMMAND_LENGTH;
	input[0][0] = '\0';
	input[1][0] = '\0';
	//open HD file and while looping
	int fd = open ("HD", O_RDWR, 660);
	int i;
	int childpid;

	printf("//-----------Simple file system--------------//\n");
	printf("tshell### \n>");
	
	while(1){
		while(1){
			int result = getline(&str, &len, stdin);

			if (result == -1)
			{
				printf("Invalid input. Exit with code -1.\n");
				return -1;
			}else if(str[0] == '\n')
			{
				cmd[0] = '\0';
			}else
			{
				str[result-1] = '\0';
				token = strtok(str, " ");
				strcpy(cmd, token);
			}
			//--------------------ls_t---------------------------//
			if(strcmp(cmd, "ls_t") == 0){
				if ((childpid = fork()) == 0)
				{
					struct dir_mapping mapping;
					printf("inode no	file type	size	name		date\n");
					int file_num = 2;
					inode = get_inode(inode.i_number);
					if (inode.i_number == 0){
						file_num = 0;
					}
					for(i=0; i<inode.file_num+file_num; i++)
					{
						lseek(fd, DATA_OFFSET + inode.direct_blk[0] * BLOCK_SIZE + i*sizeof(struct dir_mapping), SEEK_SET);
						read(fd, &mapping, sizeof(struct dir_mapping));
						current_inode = get_inode(mapping.inode_number);
						printf("%d	%10d	%10d	%5s	%30s\n",mapping.inode_number,current_inode.i_type,current_inode.i_size,mapping.dir, ctime(&current_inode.i_mtime));
					}
					break;
				} else if (childpid > 0)
				{
					wait(0);
				}
			//--------------------cd_t---------------------------//
			}else if(strcmp(cmd, "cd_t") == 0){
				token = strtok(NULL, " ");
				if(token==NULL)
				{
					printf("Invalid input. Try again please.\n");
					break;
				}
				strcpy(input[0], token);
				if(strcmp(input[0],"root")==0)
				{
					printf("Current working directory is root\n");
					current_direct_inode_number = 0;
					inode = get_inode(0);
					break;
				}
				int temp = open_t(input[0],2);
				if(temp == -1)
				{
					printf("Path not found. Please try again.\n");
				}else
				{
					if(get_inode(temp).i_type == 1)
					{
						printf("This is a file, not a directory. Current directory does not change.\n");
						break;
					}
					printf("Current directory is: %s\n",input[0]);
					current_direct_inode_number = temp;
					inode = get_inode(temp);
				}
			//--------------------mkdir_t---------------------------//
			}else if(strcmp(cmd, "mkdir_t") == 0){
				token = strtok(NULL, " ");
				strcpy(input[0], token);
				if(token==NULL)
				{
					printf("Invalid input.\n");
					break;
				}
				if ((childpid = fork()) == 0)
				{
					//child
					struct superblock sb;
					sb = get_superblock();
					int next_available_inode = sb.next_available_inode;
					int next_available_blk = sb.next_available_blk;
					struct dir_mapping mapping;
					for(i=0; i<inode.file_num+2; i++)
					{
						lseek(fd, DATA_OFFSET + inode.direct_blk[0] * BLOCK_SIZE + (i*sizeof(struct dir_mapping)), SEEK_SET);
						read(fd, (void *)&mapping, sizeof(struct dir_mapping));
						if(strcmp(input[0],mapping.dir)==0)
						{
							printf("Directory with the same name exists. New Directory replaced\n");
							int newInode = mapping.inode_number;
							set_mapping(mapping.inode_number,1);
							break;
						}
					}
					printf("Directory created, inode is: %d\n", set_inode(input[0], inode, current_direct_inode_number, next_available_inode, next_available_blk, 1));
					break;
				} else if (childpid > 0)
				{
					wait(0);
				}
				//--------------------cp_t---------------------------//
				}else if(strcmp(cmd, "cp_t") == 0){
					token = strtok(NULL, " ");
					if(token==NULL)
					{
						printf("Invalid input.\n");
						break;
					}
					strcpy(input[0], token);
					token = strtok(NULL, " ");
					if(token==NULL)
					{
						printf("Invalid input.\n");
						break;
					}
					strcpy(input[1], token);
					if ((childpid = fork()) == 0)
					{
						int i_number1 = open_t(input[0],2);
						struct inode inode1 = get_inode(i_number1);
						char * string = malloc(inode1.i_size);
						if(i_number1 == -1)
						{
							printf("File for cp_t not found.\n");
							break;
						}
						read_t(i_number1,0,string,inode1.i_size);

						int i_number2 = open_t(input[1],0);
						struct inode inode2 = get_inode(i_number2);
						if(i_number2 == -1)
						{
							struct superblock sb;
							sb = get_superblock();
							int next_available_inode = sb.next_available_inode;
							int next_available_blk = sb.next_available_blk;
							struct dir_mapping mapping;
							i_number2 = set_inode(input[1], inode, current_direct_inode_number, next_available_inode, next_available_blk, 0);
							printf("File created for cp_t, inode is: %d\n", i_number2);
						}
						write_t(i_number2,0,string,inode1.i_size);
						break;
					} else if (childpid > 0)
					{
						wait(0);
					}
			//--------------------external_cp---------------------------//
			}else if(strcmp(cmd, "external_cp") == 0){
				token = strtok(NULL, " ");
				if(token==NULL)
				{
					printf("Invalid input on external_cp para1.\n");
					break;
				}
				strcpy(input[0], token);
				token = strtok(NULL, " ");
				if(token == NULL)
				{
					printf("Invalid input on external_cp para1.\n");
					break;
				}
				strcpy(input[1], token);
				if ((childpid = fork()) == 0)
				{
					FILE *f = fopen(input[0], "rb");
					if(f)
					{
						//copy input file
						fseek(f, 0, SEEK_END);
						long fsize = ftell(f);
						fseek(f, 0, SEEK_SET);
						char *string = malloc(fsize + 1);
						fread(string, fsize, 1, f);
						fclose(f);

						//recreate new file in HD
						string[fsize] = 0;
						int i_number = open_t(input[1],0);
						if(i_number == -1)
						{
							/*
							struct superblock sb;
							sb = get_superblock();
							int next_available_inode = sb.next_available_inode;
							int next_available_blk = sb.next_available_blk;
							struct dir_mapping mapping;
							i_number = set_inode(input[1], inode, current_direct_inode_number, next_available_inode, next_available_blk, 0);
							printf("File created for external_cp, inode is: %d\n", i_number);*/
							printf("output path is not effective\n");
							break;
						}
						write_t(i_number,0,string,fsize);
						break;
					}else
					{
						printf("External file not found.\n");
						break;
					}
				} else if (childpid > 0)
				{
					wait(0);
				}
			//--------------------cat_t---------------------------//
			}else if(strcmp(cmd, "cat_t") == 0){
				token = strtok(NULL, " ");
				if(token==NULL)
				{
					printf("Invalid input.\n");
					break;
				}
				strcpy(input[0], token);
				if ((childpid = fork()) == 0)
				{
					//child go this way
					int i_number = open_t(input[0],2);
					if(i_number == -1)
					{
						printf("File not found, please try again.\n");
						break;
					}
					struct inode current_inode = get_inode(i_number);
					char* buffer = malloc(current_inode.i_size);
					read_t(current_inode.i_number,0,buffer,current_inode.i_size);
					printf("%s",buffer);
					break;
				} else if (childpid > 0)
				{
					wait(0);
				}
			}else if(strcmp(cmd, "exit") == 0){
				printf("tshell exit with -1.\n");
				exit(0);
			}else{
				printf("Command not recognized, please try again.");
			}
			printf("\n>");
		}
		//printf("\n>");
	}
}
