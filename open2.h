#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>

int open_t( const char *pathname, int flags);
struct superblock getSuperBlock();
struct inode getInode(int inode_number);
int createInode(char* name, struct inode parentInode, int parentInodeNum, int next_available_inode, int next_available_blk, int flags);
void createMapping(char* name, int inodeNum, struct inode parentInode, int parentInodeNum);
void replaceFile(int inode_number,int flags);

int open_t( const char *pathname, int flags)
{
	char* token;
	char* string;
	string = strdup(pathname);
	int found = 0;
	int inodeNum = 0; //root dir's inode number -> next dir's inode num ->...
	int targetNum;
	int distance = 0;
	int count = 0;
	char tokenArr[11][509];
	//char* dirList[10];
	int fd = open ("HD", O_RDWR, 660);
	int dirDataBlkIndex;
	int i;
	if (string != NULL)
	{
		while ((token = strsep(&string, "/")) != NULL) //split path name
		{
			if(count>=1) //after root dir, start putting in dir/file name in array
			{
				if(strcmp(token,"")!=0){
					strcpy(tokenArr[distance++],token);
					//printf("%s\n",token);
				}
			}
			count++;
		}
		count = 0;
		while(distance--)
		{
			//search through the data block(which contains dir_mapping)
			//of the dir(root first) to get dir_mapping of next dir/file
			struct dir_mapping mapping;
			struct inode inode_;
			inode_ = getInode(inodeNum); //inode of parent dir
			dirDataBlkIndex = inode_.direct_blk[0];
			if(distance==0 && (flags==0 || flags==1)) //reach the end of path, create new file (dont need to search!)
				{
					if(inode_.i_type == 1) //check if the previous file is a directory
					{
						printf("The parent \"directory\" (inode_number: %d) is just a regular file, not a directory, cannot create file here.\n", inode_.i_number);
						return -1;
					}
					struct superblock sb;
					sb = getSuperBlock();
					int next_available_inode = sb.next_available_inode;
					int next_available_blk = sb.next_available_blk;
					for(i=0; i<inode_.file_num+2; i++)
					{
						lseek(fd, DATA_OFFSET + dirDataBlkIndex * BLOCK_SIZE + (i*sizeof(struct dir_mapping)), SEEK_SET);
						read(fd, (void *)&mapping, sizeof(struct dir_mapping));
						if(strcmp(tokenArr[count],mapping.dir)==0) //there exist a file with the same name
						{
							printf("A file with the same name exists. The new file will replace it.\n");
							int newInode = mapping.inode_number;
							close(fd);
							replaceFile(mapping.inode_number,flags);
							return newInode;
						}
					}
					close(fd);
					targetNum = createInode(tokenArr[count], inode_, inodeNum, next_available_inode, next_available_blk, flags);
					//printf("outside createInode\n");
					return targetNum;
				}
			for(i=0; i<MAX_INODE; i++)
			{
				//go to the datablock that belongs to "inodeNum",
				//then start searching it MAX_INODE times
				lseek(fd, DATA_OFFSET + dirDataBlkIndex * BLOCK_SIZE + (i*sizeof(struct dir_mapping)), SEEK_SET);
				read(fd, (void *)&mapping, sizeof(struct dir_mapping));
				//got the mapping, see if the next dir/file exist
				if(strcmp(tokenArr[count],mapping.dir)==0)
				{
					found = 1;
					inodeNum = mapping.inode_number;
					//printf("tokenArr[count] is: %s; matching name is: %s; and it's inode num is: %d;", tokenArr[count], mapping.dir, mapping.inode_number);
					if(distance==0 && flags==2) //reach the end of path and flag is 2
						return inodeNum;
					break;
				}
			}
			if(found==0)
			{
				printf("Error 404: File not found, or incorrect path.\n");
				return -1;
			}
			else
			{
				count++;
				found = 0; //move on to next dir/file
			}
		}
	}
	if(found==0)
	{
		printf("Error 404: File not found, or incorrect path.\n");
		return -1;
	}
}
struct superblock getSuperBlock() //just get struct, get next_available_inode/blk in open_t
{
	int fd = open ("HD", O_RDWR, 660);
	struct superblock sb;
	lseek(fd, SB_OFFSET, SEEK_SET);
	read(fd, (void *)&sb, sizeof(struct superblock));
	//printf("getSuperBlock().nextINODE %d;\n", sb.next_available_inode);
	close(fd);
	return sb;
}
struct inode getInode(int inode_number)
{
	int fd = open ("HD", O_RDWR, 660);
	struct inode inode_;
	lseek(fd, INODE_OFFSET+inode_number*sizeof(struct inode), SEEK_SET);
	read(fd, (void *)&inode_, sizeof(struct inode));
	//printf("getInode().i_number %d;\n", inode_.i_number);
	close(fd);
	return inode_;
}
int createInode(char* name, struct inode parentInode, int parentInodeNum, int next_available_inode, int next_available_blk, int flags)
{
	//printf("inside createInode\n");
	int fd = open ("HD", O_RDWR, 660);
	int fileType = -1;
	if(flags==0) fileType = 1;
	else if(flags==1) fileType = 0;
	struct inode inode={0};
	//inode = (struct inode*)malloc(sizeof(struct inode));
	inode.i_number = next_available_inode;
	inode.i_mtime = time(NULL);
	inode.i_type = fileType;
	inode.i_size = 0;
	inode.i_blocks = 1;
	inode.direct_blk[0] = next_available_blk;
	inode.direct_blk[1] = -1;
	inode.indirect_blk = -1;
	//create dir_mapping ".", ".." if the file is a directory:
	if(fileType==0)
	{
		inode.file_num = 0;

		struct dir_mapping self={};
		strcpy(self.dir,".");
		self.inode_number = next_available_inode;

		struct dir_mapping parent={};
		strcpy(parent.dir, "..");
		parent.inode_number = parentInodeNum;

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
	//update superblock
	struct superblock sb={};
	//sb = (struct superblock*)malloc(sizeof(struct superblock));
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
	//create dir_mapping in parent dir
	createMapping(name,next_available_inode,parentInode,parentInodeNum);
	//printf("outside createMapping\n");
	return next_available_inode;
}
void createMapping(char* name, int inodeNum, struct inode parentInode, int parentInodeNum)
{
	//printf("inside createMapping\n");
	int fd = open ("HD", O_RDWR, 660);
	struct dir_mapping mapping={};
	//mapping = (struct dir_mapping*)malloc(sizeof(struct dir_mapping));
	strcpy(mapping.dir, name);
	mapping.inode_number = inodeNum;
	int fileNum;
	if(parentInodeNum==0) //root or regular directory?
	{
		fileNum=0;
	}else
	{
		fileNum=2;
	}
	lseek(fd, DATA_OFFSET + parentInode.direct_blk[0] * BLOCK_SIZE + (parentInode.file_num+fileNum) * sizeof(struct dir_mapping), SEEK_SET);
	write(fd, &mapping, sizeof(struct dir_mapping));
	//update parent dir inode
	struct inode parent={};
	parent.i_number = parentInode.i_number;
	parent.i_mtime = parentInode.i_mtime;
	parent.i_type = parentInode.i_type;
	parent.i_size = parentInode.i_size;
	parent.i_blocks = parentInode.i_blocks;
	parent.direct_blk[0] = parentInode.direct_blk[0];
	parent.direct_blk[1] = parentInode.direct_blk[1];
	parent.indirect_blk = parentInode.indirect_blk;
	parent.file_num = parentInode.file_num+1; //update here
	lseek(fd, INODE_OFFSET+parentInodeNum*sizeof(struct inode), SEEK_SET);
	write(fd, &parent, sizeof(struct inode));
	close(fd);
}
void replaceFile(int inode_number, int flags)
{
	//replace inode(take over his blocks but 0 size)
	int fileType;
	if(flags==0) fileType = 1;
	else if(flags==1) fileType = 0;

	//replace inode
	int fd = open ("HD", O_RDWR, 660);
	struct inode oldNode = getInode(inode_number);
	struct inode newNode = {};
	newNode.i_number = oldNode.i_number;
	newNode.i_mtime = time(NULL);
	newNode.i_type = fileType;
	newNode.i_size = 0;
	newNode.i_blocks = oldNode.i_blocks;
	newNode.direct_blk[0] = oldNode.direct_blk[0];
	newNode.direct_blk[1] = oldNode.direct_blk[1];
	newNode.indirect_blk = oldNode.indirect_blk;
	newNode.file_num = 0;
	lseek(fd, INODE_OFFSET+oldNode.i_number*sizeof(struct inode), SEEK_SET);
	write(fd, &newNode, sizeof(struct inode));
}
//dir_mapping = 16byte
//everytime u create file:
// update sb: get sb -> get next_available_inode/blk + update it
// create inode: put next_available_inode/blk in inode -> create it
// create dir_mapping in parent dir: create dir_mapping -> put it inside using parent inode -> update parent inode dir's file_num
// (for dir file: create dir_mapping): create dir_mapping -> put it inside using next_available_inode (for accessing it's data blk)