#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

int write_t( int inode_number, int offset, void *buf, int count);
int createDirectBlk2(struct inode inode);
int createIndirectBlk(struct inode inode);
int craeteIndirectPointer(struct inode inode);
//update next_available_blk, i_size, i_blocks, direct_blk[2], indirect_blk
int write_t( int inode_number, int offset, void *buf, int count)
{
	struct inode inode;
	struct indir_pointer pointer;
	inode = getInode(inode_number);
	if(inode.i_number != inode_number || inode.i_type == 0)
	{
		printf("File is directory or the file does not exist. Exit with code -1.\n");
		return -1; //error
	} 

	if(offset>=BLOCK_SIZE*inode.i_blocks) 
	{
		printf("Offset is at or past the end of file, no bytes are written.\n");
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
			if(blk > inode.i_blocks)
			{
				printf("Block index is > inode.i_blocks, no bytes are written.\n");
				return 0;
			}
			break;
		}
	}

	int offsetCal = offset;
	int remainingCount = count;
	int writeCount;
	int accumulateWrite = 0;
	int dirDataBlkIndex;
	int fd = open ("HD", O_RDWR, 660);
	int blkCreateCount = 0;
	while(remainingCount > 0)
	{
		inode = getInode(inode_number);
		if(blk == 0)
		{
			if (inode.direct_blk[0] == -1)
			{
				printf("Error: No direct block 1. Return bytes read (%d).\n",accumulateWrite);
				return accumulateWrite;
			}
			else
				dirDataBlkIndex = inode.direct_blk[0];
		}else if(blk == 1)
		{
			if (inode.direct_blk[1] == -1)
			{
				printf("Error: No direct block 2. Return bytes read (%d).\n",accumulateWrite);
				return accumulateWrite;
			}
			else
				dirDataBlkIndex = inode.direct_blk[1];
		}else if(blk >= 2)
		{	
			printf("%d\n", inode.direct_blk[1]);
			if (inode.indirect_blk == -1)
			{
				printf("Error: No indirect block. Return bytes read (%d).\n",accumulateWrite);
				return accumulateWrite;
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
		if(offset > 0) //first writeCount: offset > 0
			offsetCal = offset-(blk*BLOCK_SIZE); //the offset in the block

		lseek(fd, DATA_OFFSET + dirDataBlkIndex * BLOCK_SIZE + offsetCal, SEEK_SET);

		if(remainingCount < BLOCK_SIZE-offsetCal) //can finish writeCount in this block
		{
			writeCount = remainingCount;
			remainingCount -= writeCount;
		}else
		{
			writeCount = BLOCK_SIZE-offsetCal;
			remainingCount -= writeCount;
		}
		write(fd, buf + accumulateWrite, writeCount);
		accumulateWrite += writeCount;

		if(remainingCount > 0) //still have to move to next block
		{
			//move to next blk
			offset = 0; //offset is 0 now
			offsetCal = 0;
			blk++;
//CODE FOR write_t--------------------------------------------------------------------------
			//create next block and update next_available_blk here
			blkCreateCount++;
			if(blk == 1 && getInode(inode_number).direct_blk[1] == -1) //create dirblock2, update i_blocks(blkCreateCount)
			{
				createDirectBlk2(getInode(inode_number));
			}else if(blk == 2 && getInode(inode_number).indirect_blk == -1) //create indir + indir pointer, update i_blocks(blkCreateCount)
			{
				createIndirectBlk(getInode(inode_number)); //indirect datablock does not count as data block in i_blocks
				craeteIndirectPointer(getInode(inode_number));
			}else if(blk > 2) //create indir pointer, update i_blocks(blkCreateCount)
			{
				craeteIndirectPointer(getInode(inode_number));
			}
		}
	}
//update i_size (accumulateWrite)------------------------------------------------------------
	struct inode updatedInode={};
	updatedInode.i_number = inode.i_number;
	updatedInode.i_mtime = inode.i_mtime;
	updatedInode.i_type = inode.i_type;
	updatedInode.i_size = inode.i_size+accumulateWrite; //update
	updatedInode.i_blocks = inode.i_blocks;
	updatedInode.direct_blk[0] = inode.direct_blk[0];
	updatedInode.direct_blk[1] = inode.direct_blk[1];
	updatedInode.indirect_blk = inode.indirect_blk;
	lseek(fd, INODE_OFFSET + inode.i_number*sizeof(struct inode), SEEK_SET);
	write(fd, &updatedInode, sizeof(struct inode));

//CODE FOR write_t end-----------------------------------------------------------------------
	close(fd);
	return accumulateWrite;
}

int createDirectBlk2(struct inode inode){
	struct superblock sb;
	sb = getSuperBlock();
	int next_available_blk = sb.next_available_blk;
	if(next_available_blk == MAX_DATA_BLK)
	{
		printf("Reached MAX_DATA_BLK! Abort.\n");
		return -1;
	}
	int fd = open ("HD", O_RDWR, 660);
	//update inode
	struct inode updatedInode={};
	updatedInode.i_number = inode.i_number;
	updatedInode.i_mtime = inode.i_mtime;
	updatedInode.i_type = inode.i_type;
	updatedInode.i_size = inode.i_size;
	updatedInode.i_blocks = 2;
	updatedInode.direct_blk[0] = inode.direct_blk[0];
	updatedInode.direct_blk[1] = next_available_blk; //update
	updatedInode.indirect_blk = inode.indirect_blk;
	lseek(fd, INODE_OFFSET + inode.i_number*sizeof(struct inode), SEEK_SET);
	//printf("d2: updatedInode.direct_blk[1] = %d\n", updatedInode.direct_blk[1]);
	write(fd, &updatedInode, sizeof(struct inode));

	//update superblock
	struct superblock updatedSb={};
	updatedSb.inode_offset = INODE_OFFSET;
	updatedSb.data_offset = DATA_OFFSET;
	updatedSb.max_inode = MAX_INODE;
	updatedSb.max_data_blk = MAX_DATA_BLK;
	updatedSb.blk_size = BLOCK_SIZE;
	updatedSb.next_available_inode = sb.next_available_inode;
	updatedSb.next_available_blk = (next_available_blk+1); //update here

	lseek(fd, SB_OFFSET, SEEK_SET);
	write(fd, &updatedSb, sizeof(struct superblock));
	
	close(fd);
	return next_available_blk;
}

int createIndirectBlk(struct inode inode){
	struct superblock sb;
	sb = getSuperBlock();
	int next_available_blk = sb.next_available_blk;
	int fd = open ("HD", O_RDWR, 660);
	if(next_available_blk == MAX_DATA_BLK)
	{
		printf("Reached MAX_DATA_BLK! Abort.\n");
		return -1;
	}
	//update inode
	struct inode updatedInode={};
	updatedInode.i_number = inode.i_number;
	updatedInode.i_mtime = inode.i_mtime;
	updatedInode.i_type = inode.i_type;
	updatedInode.i_size = inode.i_size;
	updatedInode.i_blocks = 2; //dont change, indirect block does not count as block here
	updatedInode.direct_blk[0] = inode.direct_blk[0];
	updatedInode.direct_blk[1] = inode.direct_blk[1];
	updatedInode.indirect_blk = next_available_blk; //update
	lseek(fd, INODE_OFFSET + inode.i_number*sizeof(struct inode), SEEK_SET);
	//printf("Indirect updatedInode.indirect_blk = %d\n", updatedInode.indirect_blk);
	write(fd, &updatedInode, sizeof(struct inode));

	//update superblock
	struct superblock updatedSb={};
	updatedSb.inode_offset = INODE_OFFSET;
	updatedSb.data_offset = DATA_OFFSET;
	updatedSb.max_inode = MAX_INODE;
	updatedSb.max_data_blk = MAX_DATA_BLK;
	updatedSb.blk_size = BLOCK_SIZE;
	updatedSb.next_available_inode = sb.next_available_inode;
	updatedSb.next_available_blk = (next_available_blk+1); //update here

	lseek(fd, SB_OFFSET, SEEK_SET);
	write(fd, &updatedSb, sizeof(struct superblock));
	
	close(fd);
	return next_available_blk;
}


int craeteIndirectPointer(struct inode inode){
	struct superblock sb;
	sb = getSuperBlock();
	int next_available_blk = sb.next_available_blk;
	int fd = open ("HD", O_RDWR, 660);
	if(next_available_blk == MAX_DATA_BLK)
	{
		printf("Reached MAX_DATA_BLK! Abort.\n");
		return -1;
	}
	//update inode
	struct inode updatedInode={};
	updatedInode.i_number = inode.i_number;
	updatedInode.i_mtime = inode.i_mtime;
	updatedInode.i_type = inode.i_type;
	updatedInode.i_size = inode.i_size;
	updatedInode.i_blocks = inode.i_blocks+1; //update
	updatedInode.direct_blk[0] = inode.direct_blk[0];
	updatedInode.direct_blk[1] = inode.direct_blk[1];
	updatedInode.indirect_blk = inode.indirect_blk; 
	lseek(fd, INODE_OFFSET + inode.i_number*sizeof(struct inode), SEEK_SET);
	write(fd, &updatedInode, sizeof(struct inode));

	//update indirect data block
	struct indir_pointer pointer;
	pointer.blkIndex = next_available_blk;
	lseek(fd, DATA_OFFSET + inode.indirect_blk*BLOCK_SIZE + (inode.i_blocks-2)*sizeof(struct indir_pointer), SEEK_SET);
	write(fd, &pointer, sizeof(struct indir_pointer));

	//update superblock
	struct superblock updatedSb={};
	updatedSb.inode_offset = INODE_OFFSET;
	updatedSb.data_offset = DATA_OFFSET;
	updatedSb.max_inode = MAX_INODE;
	updatedSb.max_data_blk = MAX_DATA_BLK;
	updatedSb.blk_size = BLOCK_SIZE;
	updatedSb.next_available_inode = sb.next_available_inode;
	updatedSb.next_available_blk = (next_available_blk+1); //update here

	lseek(fd, SB_OFFSET, SEEK_SET);
	write(fd, &updatedSb, sizeof(struct superblock));
	
	close(fd);
	return next_available_blk;
}


// The number of bytes written may be less than count if there is insufficient
// space or the maximum size of a file has been
// achieved. On success, the number of bytes written is returned (zero indicates
// nothing was written). On error, -1 is returned.