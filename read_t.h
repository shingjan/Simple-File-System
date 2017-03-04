#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

int read_t( int inode_number, int offset, void *buf, int count);

int read_t( int inode_number, int offset, void *buf, int count)
{
	struct inode inode;
	struct indir_pointer pointer;
	inode = getInode(inode_number);
	if(inode.i_number != inode_number || inode.i_type == 0) 
	{
		printf("File is directory or the file does not exist. Exit with code -1.\n");
		return -1; //error
	}
	if(offset>=inode.i_size) 
	{
		printf("Offset is at or past the end of file, no bytes are read.\n");
		return 0;
	}
	//offset within the size of file
	int blk;
	int i;
	for(i=0;i<MAX_DATA_BLK;i++) //find which block lies the offset 
	{
		if(offset<BLOCK_SIZE*(i+1))
		{
			blk=i;
			break;
		}
	}

	int offsetCal = offset;
	int remainingCount = count;
	int readCount;
	int accumulateRead = 0;
	int dirDataBlkIndex;
	int fd = open ("HD", O_RDWR, 660);

	while(remainingCount > 0)
	{
		if(blk == 0)
		{
			if (inode.direct_blk[0] == -1)
			{
				printf("Error: No direct block 1. Return bytes read.\n");
				return accumulateRead;
			}
			else
				dirDataBlkIndex = inode.direct_blk[0];
		}else if(blk == 1)
		{
			if (inode.direct_blk[1] == -1)
			{
				printf("Error: No direct block 2. Return bytes read.\n");
				return accumulateRead;
			}
			else
				dirDataBlkIndex = inode.direct_blk[1];
		}else if(blk >= 2)
		{
			if (inode.indirect_blk == -1)
			{
				printf("Error: No indirect block. Return bytes read.\n");
				return accumulateRead;
			}
			else
				{
					//find the suitable block:
					//goto the indirect pointer datablock, get the i-th pointer
					//dirDataBlkIndex = that pointer(index)
					lseek(fd, DATA_OFFSET + inode.indirect_blk * BLOCK_SIZE + (blk-2)*sizeof(struct indir_pointer), SEEK_SET);
					read(fd, (void *)&pointer, sizeof(struct indir_pointer));
					dirDataBlkIndex = pointer.blkIndex;
				}			
		}
		if(offset > 0) //first readCount: offset > 0
			offsetCal = offset-(blk*BLOCK_SIZE); //the offset in the block

		lseek(fd, DATA_OFFSET + dirDataBlkIndex * BLOCK_SIZE + offsetCal, SEEK_SET);

		if(remainingCount < BLOCK_SIZE-offsetCal) //can finish readCount in this block
		{
			readCount = remainingCount;
			remainingCount -= readCount;
		}else
		{
			readCount = BLOCK_SIZE-offsetCal;
			remainingCount -= readCount;
		}

		//read(fd, buf + accumulateRead, readCount);
		read(fd, buf+accumulateRead, readCount);
		accumulateRead += readCount;
		//printf("%s\n",str);
		if(remainingCount > 0) //still have to move to next block
		{
			//move to next blk
			offset = 0; //offset is 0 now
			offsetCal = 0;
			blk++;
		}
	}
	close(fd);
	return accumulateRead;
}

// If offset is at or past the end of file, no bytes are read, and read_t() returns zero. 
// On success, the number of bytes readCount is returned (zero indicates end of file), and on error, -1 is returned.