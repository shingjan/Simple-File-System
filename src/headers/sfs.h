
#define SB_OFFSET 4096
#define INODE_OFFSET 8192
#define DATA_OFFSET 8388608
#define MAX_INODE 170
#define MAX_DATA_BLK 20480
#define BLOCK_SIZE 4096
#define MAX_NESTING_DIR 10
#define MAX_COMMAND_LENGTH 50

struct superblock
{
	int inode_offset;
	int data_offset;
	int max_inode;
	int max_data_blk;
	int next_available_inode;
	int next_available_blk;
	int blk_size;
};

struct inode
{
	int i_number;
	time_t i_mtime;
	int i_type;
	int i_size;
	int i_blocks;
	int direct_blk[2];
	int indirect_blk;
	int file_num;
};

typedef struct dir_mapping
{
	char dir[10];
	int inode_number;
}DIR_NODE;

struct indir_pointer
{
	int blkIndex; 
};
