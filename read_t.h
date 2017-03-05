int read_t( int inode_number, int offset, void *buf, int flags)
{
	struct inode inode;
	struct indir_pointer pointer;
	inode = get_inode(inode_number);

	//base case
	if(inode.i_number != inode_number || inode.i_type == 0)
	{
		printf("Target file is a directory or file does not exist. Exit with code -1.\n");
		return -1;
	}
	if(offset >= inode.i_size)
	{
		printf("Offset exceeds the size of file, no bytes are read.\n");
		return 0;
	}

	//offset within the size of file
	int blk;
	int i;
	//-----------------find the block number------------------//
	for(i=0;i<MAX_DATA_BLK;i++)
	{
		if(offset<BLOCK_SIZE*(i+1))
		{
			blk=i;
			break;
		}
	}
	//-----------------looking for the data------------------//
	int read_offset = offset;
	int read_count = flags;
	int read_count2;
	int result = 0;
	int dirext_data_block_index;
	int fd = open ("HD", O_RDWR, 660);

	while(read_count > 0)
	{
		if(blk == 0)
		{
			if (inode.direct_blk[0] == -1)
			{
				printf("Error: No direct block 1. Return tuncated results.\n");
				return result;
			}
			else
				dirext_data_block_index = inode.direct_blk[0];
		}else if(blk == 1)
		{
			if (inode.direct_blk[1] == -1)
			{
				printf("Second direct block 2. Return tuncated results.\n");
				return result;
			}
			else
				dirext_data_block_index = inode.direct_blk[1];
		}else if(blk >= 2)
		{
			if (inode.indirect_blk == -1)
			{
				printf("Indirect block 2 Not found. Return tuncated results.\n");
				return result;
			}
			else
			{
				lseek(fd, DATA_OFFSET + inode.indirect_blk * BLOCK_SIZE + (blk-2)*sizeof(struct indir_pointer), SEEK_SET);
				read(fd, (void *)&pointer, sizeof(struct indir_pointer));
				dirext_data_block_index = pointer.blkIndex;
			}
		}
		if(offset > 0)
		{
			read_offset = offset-(blk*BLOCK_SIZE);
		}

		lseek(fd, DATA_OFFSET + dirext_data_block_index * BLOCK_SIZE + read_offset, SEEK_SET);

		if(read_count < BLOCK_SIZE-read_offset)
		{
			read_count2 = read_count;
			read_count -= read_count2;
		}else
		{
			read_count2 = BLOCK_SIZE-read_offset;
			read_count -= read_count2;
		}

		read(fd, buf+result, read_count2);
		result += read_count2;

		if(read_count > 0)
		{
			offset = 0;
			read_offset = 0;
			blk++;
		}
	}
	close(fd);
	return result;
}
