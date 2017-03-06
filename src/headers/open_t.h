struct superblock get_superblock()
{
	int fd = open ("HD", O_RDWR, 660);
	struct superblock sb;
	lseek(fd, SB_OFFSET, SEEK_SET);
	read(fd, (void *)&sb, sizeof(struct superblock));
	close(fd);
	return sb;
}

struct inode get_inode(int inode_number)
{
	int fd = open ("HD", O_RDWR, 660);
	struct inode inode_;
	lseek(fd, INODE_OFFSET+inode_number*sizeof(struct inode), SEEK_SET);
	read(fd, (void *)&inode_, sizeof(struct inode));
	close(fd);
	return inode_;
}

void set_inode_mapping(char* name, int inodeNum, struct inode parent_inode, int parent_inode_number)
{
	int fd = open ("HD", O_RDWR, 660);
	struct dir_mapping mapping={};
	strcpy(mapping.dir, name);
	mapping.inode_number = inodeNum;
	int fileNum;
	if(parent_inode_number==0)
	{
		fileNum=0;
	}else
	{
		fileNum=2;
	}
	lseek(fd, DATA_OFFSET + parent_inode.direct_blk[0] * BLOCK_SIZE + (parent_inode.file_num+fileNum) * sizeof(struct dir_mapping), SEEK_SET);
	write(fd, &mapping, sizeof(struct dir_mapping));

	struct inode parent={};
	parent.i_number = parent_inode.i_number;
	parent.i_mtime = parent_inode.i_mtime;
	parent.i_type = parent_inode.i_type;
	parent.i_size = parent_inode.i_size;
	parent.i_blocks = parent_inode.i_blocks;
	parent.direct_blk[0] = parent_inode.direct_blk[0];
	parent.direct_blk[1] = parent_inode.direct_blk[1];
	parent.indirect_blk = parent_inode.indirect_blk;
	parent.file_num = parent_inode.file_num+1; //update here
	lseek(fd, INODE_OFFSET+parent_inode_number*sizeof(struct inode), SEEK_SET);
	write(fd, &parent, sizeof(struct inode));
	close(fd);
}

int set_inode(char* name, struct inode parent_inode, int parent_inode_number, int next_available_inode, int next_available_blk, int file_type)
{
	int fd = open ("HD", O_RDWR, 660);
	int fileType = -1;
	if(file_type==0)
		fileType = 1;
	else if(file_type==1)
		fileType = 0;

	//----------------make inode----------------//
	struct inode inode={0};
	inode.i_number = next_available_inode;
	inode.i_mtime = time(NULL);
	inode.i_type = fileType;
	inode.i_size = 0;
	inode.i_blocks = 1;
	inode.direct_blk[0] = next_available_blk;
	inode.direct_blk[1] = -1;
	inode.indirect_blk = -1;

	if(fileType==0)
	{
		inode.file_num = 0;

		struct dir_mapping self={};
		strcpy(self.dir,".");
		self.inode_number = next_available_inode;

		struct dir_mapping parent={};
		strcpy(parent.dir, "..");
		parent.inode_number = parent_inode_number;

		lseek(fd, DATA_OFFSET + inode.direct_blk[0]*BLOCK_SIZE, SEEK_SET);
		write(fd, &self, sizeof(struct dir_mapping));
		lseek(fd, DATA_OFFSET + inode.direct_blk[0]*BLOCK_SIZE + sizeof(struct dir_mapping), SEEK_SET);
		write(fd, &parent, sizeof(struct dir_mapping));
	}
	else
	{
		inode.file_num = 0;
	}
	lseek(fd, INODE_OFFSET+sizeof(struct inode) * next_available_inode, SEEK_SET);
	write(fd, &inode, sizeof(struct inode));

	//-------------------make super block----------------------//
	struct superblock sb={};
	sb.inode_offset = INODE_OFFSET;
	sb.data_offset = DATA_OFFSET;
	sb.max_inode = MAX_INODE;
	sb.max_data_blk = MAX_DATA_BLK;
	sb.blk_size = BLOCK_SIZE;
	sb.next_available_inode = (next_available_inode+1);
	sb.next_available_blk = (next_available_blk+1);
	lseek(fd, SB_OFFSET, SEEK_SET);
	write(fd, &sb, sizeof(struct superblock));
	close(fd);
	set_inode_mapping(name,next_available_inode,parent_inode,parent_inode_number);
	return next_available_inode;
}



void set_mapping(int inode_number, int file_type)
{
	int fileType;
	if(file_type==0)
		fileType = 1;
	else if(file_type==1)
		fileType = 0;

	int fd = open ("HD", O_RDWR, 660);
	struct inode prev_node = get_inode(inode_number);
	struct inode temp_node = {};
	temp_node.i_number = prev_node.i_number;
	temp_node.i_mtime = time(NULL);
	temp_node.i_type = fileType;
	temp_node.i_size = 0;
	temp_node.i_blocks = prev_node.i_blocks;
	temp_node.direct_blk[0] = prev_node.direct_blk[0];
	temp_node.direct_blk[1] = prev_node.direct_blk[1];
	temp_node.indirect_blk = prev_node.indirect_blk;
	temp_node.file_num = 0;
	lseek(fd, INODE_OFFSET+prev_node.i_number*sizeof(struct inode), SEEK_SET);
	write(fd, &temp_node, sizeof(struct inode));
}

//-------------------------------open_t-----------------------------//
int open_t(const char *pathname, int file_type)
{
	char* token;
	char* path;
	strcpy(pathname, path);
	char token_array[MAX_NESTING_DIR][MAX_COMMAND_LENGTH];
	int fd = open ("HD", O_RDWR, 660);
	int direct_data_block_index;

	//all parameters
	int open_count = 0;
	int open_count2 = 0;
	int inodeNum = 0;
	int targetNum;
	int depth = 0;

	if (path == NULL)
	{
		printf("Invalid input, exit with code -1 ");
		return -1;
	}

	while ((token = strsep(&path, "/")) != NULL)
	{
			if(open_count>=1)
			{
				if(strcmp(token,"")!=0){
					strcpy(token_array[depth++],token);
				}
			}
		open_count++;
	}
	open_count = 0;
	int i;
	while(depth--)
		{
			struct dir_mapping mapping;
			struct inode inode_;
			inode_ = get_inode(inodeNum);
			direct_data_block_index = inode_.direct_blk[0];
			if(depth==0 && (file_type==0 || file_type==1))
				{
					if(inode_.i_type == 1)
					{
						printf("The parent \"directory\" (inode_number: %d) is just a regular file, not a directory, cannot create file here.\n", inode_.i_number);
						return -1;
					}
					struct superblock sb;
					sb = get_superblock();
					int next_available_inode = sb.next_available_inode;
					int next_available_blk = sb.next_available_blk;
					for(i=0; i<inode_.file_num+2; i++)
					{
						lseek(fd, DATA_OFFSET + direct_data_block_index * BLOCK_SIZE + (i*sizeof(struct dir_mapping)), SEEK_SET);
						read(fd, (void *)&mapping, sizeof(struct dir_mapping));
						if(strcmp(token_array[open_count],mapping.dir)==0)
						{
							printf("Overriding file with same name.\n");
							int newInode = mapping.inode_number;
							close(fd);
							set_mapping(mapping.inode_number,file_type);
							return newInode;
						}
					}
					close(fd);
					targetNum = set_inode(token_array[open_count], inode_, inodeNum, next_available_inode, next_available_blk, file_type);
					return targetNum;
				}

			for(i=0; i<MAX_INODE; i++)
			{
				lseek(fd, DATA_OFFSET + direct_data_block_index * BLOCK_SIZE + (i*sizeof(struct dir_mapping)), SEEK_SET);
				read(fd, (void *)&mapping, sizeof(struct dir_mapping));
				if(strcmp(token_array[open_count],mapping.dir)==0)
				{
					open_count2 = 1;
					inodeNum = mapping.inode_number;
					if(depth==0 && file_type==2)
						return inodeNum;
					break;
				}
			}
			if(open_count2==0)
			{
				printf("File not found, or path is incorrect.\n");
				return -1;
			}
			else
			{
				open_count++;
				open_count2 = 0;
			}
		}

	if(open_count2==0)
	{
		printf("File not found, or path name ineffective.\n");
		return -1;
	}
}
