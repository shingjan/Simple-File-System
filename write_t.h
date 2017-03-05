int set_direct_block(struct inode inode){
	struct superblock sb;
	sb = get_superblock();
	int next_available_blk = sb.next_available_blk;
	if(next_available_blk == MAX_DATA_BLK)
	{
		printf("Reached MAX_DATA_BLK! Abort.\n");
		return -1;
	}
	int fd = open ("HD", O_RDWR, 660);

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

	write(fd, &updatedInode, sizeof(struct inode));


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

int set_indirect_block(struct inode inode){
	struct superblock sb;
	sb = get_superblock();
	int next_available_blk = sb.next_available_blk;
	int fd = open ("HD", O_RDWR, 660);
	if(next_available_blk == MAX_DATA_BLK)
	{
		printf("Reached MAX_DATA_BLK! Abort.\n");
		return -1;
	}

	struct inode updatedInode={};
	updatedInode.i_number = inode.i_number;
	updatedInode.i_mtime = inode.i_mtime;
	updatedInode.i_type = inode.i_type;
	updatedInode.i_size = inode.i_size;
	updatedInode.i_blocks = 2;
	updatedInode.direct_blk[0] = inode.direct_blk[0];
	updatedInode.direct_blk[1] = inode.direct_blk[1];
	updatedInode.indirect_blk = next_available_blk;
	lseek(fd, INODE_OFFSET + inode.i_number*sizeof(struct inode), SEEK_SET);
	write(fd, &updatedInode, sizeof(struct inode));

	struct superblock updatedSb={};
	updatedSb.inode_offset = INODE_OFFSET;
	updatedSb.data_offset = DATA_OFFSET;
	updatedSb.max_inode = MAX_INODE;
	updatedSb.max_data_blk = MAX_DATA_BLK;
	updatedSb.blk_size = BLOCK_SIZE;
	updatedSb.next_available_inode = sb.next_available_inode;
	updatedSb.next_available_blk = (next_available_blk+1);

	//set the offset
	lseek(fd, SB_OFFSET, SEEK_SET);
	write(fd, &updatedSb, sizeof(struct superblock));
	close(fd);
	return next_available_blk;
}


int set_indirect_offset(struct inode inode){
	struct superblock sb;
	sb = get_superblock();
	int next_available_blk = sb.next_available_blk;
	int fd = open ("HD", O_RDWR, 660);
	if(next_available_blk == MAX_DATA_BLK)
	{
		printf("Reached MAX_DATA_BLK! Abort.\n");
		return -1;
	}

	struct inode updatedInode={};
	updatedInode.i_number = inode.i_number;
	updatedInode.i_mtime = inode.i_mtime;
	updatedInode.i_type = inode.i_type;
	updatedInode.i_size = inode.i_size;
	updatedInode.i_blocks = inode.i_blocks+1; //update
	updatedInode.direct_blk[0] = inode.direct_blk[0];
	updatedInode.direct_blk[1] = inode.direct_blk[1];
	updatedInode.indirect_blk = inode.indirect_blk;
	//set the offset
	lseek(fd, INODE_OFFSET + inode.i_number*sizeof(struct inode), SEEK_SET);
	write(fd, &updatedInode, sizeof(struct inode));

	//update the offset
	struct indir_pointer pointer;
	pointer.blkIndex = next_available_blk;
	lseek(fd, DATA_OFFSET + inode.indirect_blk*BLOCK_SIZE + (inode.i_blocks-2)*sizeof(struct indir_pointer), SEEK_SET);
	write(fd, &pointer, sizeof(struct indir_pointer));

	//update superblock offset
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
//-------------------------write_t----------------------------//
int write_t( int inode_number, int offset, void *buf, int count)
{
	struct inode inode;
	struct indir_pointer pointer;
	inode = get_inode(inode_number);
	if(inode.i_number != inode_number || inode.i_type == 0)
	{
		printf("File is directory or the file does not exist. Exit with code -1.\n");
		return -1;
	}

	if(offset>=BLOCK_SIZE*inode.i_blocks)
	{
		printf("Offset is at or past the end of file, no bytes are written.\n");
		return 0;
	}

	int blk;
	int i;
	for(i=0;i<MAX_DATA_BLK;i++)
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

	int write_offset = offset;
	int remainingCount = count;
	int writeCount;
	int write_count = 0;
	int direct_data_block_index;
	int fd = open ("HD", O_RDWR, 660);
	int blkCreateCount = 0;
	while(remainingCount > 0)
	{
		inode = get_inode(inode_number);
		if(blk == 0)
		{
			if (inode.direct_blk[0] == -1)
			{
				printf("Error: No direct block 1. Return bytes read (%d).\n",write_count);
				return write_count;
			}
			else
				direct_data_block_index = inode.direct_blk[0];
		}else if(blk == 1)
		{
			if (inode.direct_blk[1] == -1)
			{
				printf("Error: No direct block 2. Return bytes read (%d).\n",write_count);
				return write_count;
			}
			else
				direct_data_block_index = inode.direct_blk[1];
		}else if(blk >= 2)
		{
			printf("%d\n", inode.direct_blk[1]);
			if (inode.indirect_blk == -1)
			{
				printf("Error: No indirect block. Return bytes read (%d).\n",write_count);
				return write_count;
			}
			else
				{
					lseek(fd, DATA_OFFSET + inode.indirect_blk * BLOCK_SIZE + (blk-2)*sizeof(struct indir_pointer), SEEK_SET);
					read(fd, (void *)&pointer, sizeof(struct indir_pointer));
					direct_data_block_index = pointer.blkIndex;
				}
		}

		if(offset > 0)
			write_offset = offset-(blk*BLOCK_SIZE);

		lseek(fd, DATA_OFFSET + direct_data_block_index * BLOCK_SIZE + write_offset, SEEK_SET);

		if(remainingCount < BLOCK_SIZE-write_offset)
		{
			writeCount = remainingCount;
			remainingCount -= writeCount;
		}
		else
		{
			writeCount = BLOCK_SIZE-write_offset;
			remainingCount -= writeCount;
		}
		write(fd, buf + write_count, writeCount);
		write_count += writeCount;

		if(remainingCount > 0)
		{
			offset = 0;
			write_offset = 0;
			blk++;
			blkCreateCount++;
			if(blk == 1 && get_inode(inode_number).direct_blk[1] == -1)
			{
				set_direct_block(get_inode(inode_number));
			}else if(blk == 2 && get_inode(inode_number).indirect_blk == -1)
			{
				set_indirect_block(get_inode(inode_number));
				set_indirect_offset(get_inode(inode_number));
			}else if(blk > 2)
			{
				set_indirect_offset(get_inode(inode_number));
			}
		}
	}

	struct inode updatedInode={};
	updatedInode.i_number = inode.i_number;
	updatedInode.i_mtime = inode.i_mtime;
	updatedInode.i_type = inode.i_type;
	updatedInode.i_size = inode.i_size+write_count;
	updatedInode.i_blocks = inode.i_blocks;
	updatedInode.direct_blk[0] = inode.direct_blk[0];
	updatedInode.direct_blk[1] = inode.direct_blk[1];
	updatedInode.indirect_blk = inode.indirect_blk;
	lseek(fd, INODE_OFFSET + inode.i_number*sizeof(struct inode), SEEK_SET);
	write(fd, &updatedInode, sizeof(struct inode));

	close(fd);
	return write_count;
}
