#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include "sfs.h"

int main(int argc, char *argv[])
{
	int fd = open (argv[1], O_RDWR, 660);
	if (fd == -1)
	{
		printf("File doesn't exist, exit with code -1.\n");
		return -1;
	}

	struct superblock* super_block;
	struct inode* inode;
	struct dir_mapping* root_directory;

	super_block = (struct superblock*)malloc(sizeof(struct superblock));
	super_block->inode_offset = INODE_OFFSET;
	super_block->data_offset = DATA_OFFSET;
	super_block->max_inode = MAX_INODE;
	super_block->max_data_blk = MAX_DATA_BLK;
	super_block->blk_size = BLOCK_SIZE;
	super_block->next_available_inode = 0;
	super_block->next_available_blk = 0;

	lseek(fd, SB_OFFSET, SEEK_SET);
	write(fd, (void *)super_block, sizeof(struct superblock));

	inode = (struct inode*)malloc(sizeof(struct inode));
	inode->i_number = 0;
	inode->i_mtime = time(NULL);
	inode->i_type = 0;
	inode->i_size = 0;
	inode->i_blocks = 1;
	inode->direct_blk[0] = 0;
	inode->direct_blk[1] = -1;
	inode->indirect_blk = -1;
	inode->file_num = 0;

	lseek(fd, INODE_OFFSET, SEEK_SET);
	write(fd, (void *)inode, sizeof(struct inode));

	root_directory = (struct dir_mapping*)malloc(sizeof(struct dir_mapping));
	strcpy(root_directory->dir,".");
	root_directory->inode_number=0;
	lseek(fd, DATA_OFFSET, SEEK_SET);
	write(fd, (void *)root_directory, sizeof(struct dir_mapping));

	super_block = (struct superblock*)malloc(sizeof(struct superblock));
	super_block->inode_offset = INODE_OFFSET;
	super_block->data_offset = DATA_OFFSET;
	super_block->max_inode = MAX_INODE;
	super_block->max_data_blk = MAX_DATA_BLK;
	super_block->blk_size = BLOCK_SIZE;
	super_block->next_available_inode = 1;
	super_block->next_available_blk = 1;
	lseek(fd, SB_OFFSET, SEEK_SET);
	write(fd, (void *)super_block, sizeof(struct superblock));

	printf("File system offset reset!.\n");
	return 0;
}
