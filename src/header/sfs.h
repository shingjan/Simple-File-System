#ifndef _LAYOUT_
#define _LAYOUT_

char *file_name="HD";


#define SB_OFFSET 4096			/* The offset of superblock region*/
#define INODE_OFFSET 8192		/* The offset of inode region */
#define DATA_OFFSET 8388608		/* The offset of data region */
#define MAX_INODE 170			/* The maximum number of inode s */
#define MAX_DATA_BLK 20480		/* The maximum number of blockblockblock s */
#define BLOCK_SIZE 4096			/* The size per block */
#define MAX_NESTING_DIR 10 		/* The nesting number of directory */
#define MAX_COMMAND_LENGTH 50	/* The maximum command length */

#define K 1024	/* The maximum command length */

struct superblock /*The key information of filesystem */
{
	int inode_offset;			/* The start offset of the inode region */
	int data_offset;			/* The start offset of the data region */
	int max_inode;				/* The maximum number of inodes */
	int max_data_blk;			/* The maximum number of data blocks */
	int next_available_inode;	/* The index of the next free inode */
	int next_available_blk;		/* The index of the next free block */
	int blk_size;				/* The size per block */
};

struct inode /* The structure of inode, each file has only one */
{
	int i_number;		/* The inode number */
	time_t i_mtime;		/* Creation time of inode*/ /* Creation time of inode*/
	int i_type;			/* Regular file for 1, directory 0 */
	int i_size;			/* The size of file */
	int i_blocks;		/* The total numbers of data blocks */
	int direct_blk[2];	/*Two direct data block pointers */
	int indirect_blk;	/*One indirect data block pointer */
	int file_num;		/* The number of file in directory, it is 0 if file*/
};



typedef struct dir_mapping /* Record file information in directory */
{
	char dir[10];/* The file name in current directory */
	int inode_number; /* The corresponding inode number */
}DIR_NODE;


struct file_blk /* The structure of file block */
{
	int size;		/* size of content */
	int blk_pos;	/* Block ID of content*/
};


typedef   unsigned   char   byte;	/* Byte */

struct path_ans 				/*Path Analysis Result*/
{
	int target_inode;			/* The target directory inode number */
	int isExist;				/* Whether the target file exist yet */
	char target_name[10];		/* Name of Target File */
};

#endif

int cur = 0;


// Get Value from Super Block(Flags)
int getSuperBlock(int);
// Set Value in Super Block(Flags, Value)
void setSuperBlock(int, int);


// Read Inode(Inode_Number)
struct inode *read_inode(int );
// Write Inode(Inode_Number, Inode);
void write_inode(int , struct inode *);


// Read Block(Block_Number)
struct dir_mapping *read_block(int );
// Write Block(Block_Number, Block)
void write_block(int , struct dir_mapping *);


// Update Parent List(Parent_Block, New_Block)
void update_par_list(struct dir_mapping *,struct dir_mapping *);
// Return Parent Inode Number(int Inode_Number)
int parent_inode(int);
// Return Child Position(Content_List_Block_Number, Name of Child, File_Number_of_Parent)
int get_child_pos(int,char*, int);
// Print Child List(Content_List_Block,File_Number_of_Parent)
void get_child_list(int , int );

// Return Child Inode Number(Parent_Inode_Number,Child Name)
int child_node(int , char* );
// Return Child Inode Number(Parent_Block_Number,Child Name)
int get_dir_node(int, char*);


// Create a Directory(Parent_Block_Number, Directory_Name,Flags)
// Flags: 0-Create New 1-Replace Exisit
int create_dir(int , char* , int );
// Create a File(Parent_Block_Number, Directory_Name,Flags)
// Flags: 0-Create New 1-Replace Exisit
int create_file(int , char* , int );

// Update a Content Info of File to File List
// (File_Indirect_Block,Offset, Size, Target_Block)
void update_content_list(int , int , int ,int );
// Read a Content Info of File from File List
// (File_Indirect_Block,Offset)
struct file_blk *read_content_list(int , int );

// Get File Content(Block_Number, Offset, Buff, Count)
void get_file_content(int ,int ,void *, int );
// Set File Content(Block_Number, Offset, Buff, Count)
void set_file_content(int ,int ,void *, int );


// Path Analysis(Path, Current_Directory)
struct path_ans *path_analysis(char* ,int );






// Init Superblock
void init_superblock();

// Read From a Real File(Path, Inode)
void read_real_file(char* , int );
// Write to a Real File(Path, Inode)
void write_real_file(char* , int );
// Inside Copy(Source Inode, Target Inode)
void inside_copy(int sou, int des);


// Init File System(Drive_Name)
int init_the_file_system(char *);






//=====================================================


// int open_t(const char *pathname, int flags)
int open_t(const char *, int );
// int read_t(int node_number, int offset, void *buff,int count)
int read_t(int , int , void *,int );
// int write_t(int node_number, int offset, void *buff,int count)
int write_t(int , int , void *,int );


//=====================================================


// mkfs_t
void make_file(char* );
// mkdir_t
void make_dir(char* );
// cd_t
void cd_path(char* );
// ls_t
void ls_command();
// cat_t
void cat(char* );
// external_cp(External_Path, Tshell_Path)
void external(char* , char* );
// internal_cp(External_Path, Tshell_Path)
void internal(char* , char* );
// cp_t
void inner_copy(char* , char* );

int getSuperBlock(int flags){

	//printf(">>>Load Super Block: %d<<<\n",flags);


	int fd=open(file_name, O_RDONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	lseek(fd, SB_OFFSET, SEEK_SET);


	struct superblock *super_block = (struct superblock*)malloc(sizeof(struct superblock));

	read(fd, (struct superblock *)super_block, sizeof(struct superblock));

	int ans = -1;

	switch (flags){
		case 0:
			ans = super_block->inode_offset;
			//printf("inode_offset = %d\n", ans);
			break;
		case 1:
			ans = super_block->max_inode;
			//printf("max_inode = %d\n", ans);
			break;
		case 2:
			ans = super_block->max_data_blk;
			//printf("max_data_blk = %d\n", ans);
			break;
		case 3:
			ans = super_block->next_available_inode;
			//printf("next_available_inode = %d\n", ans);
			break;
		case 4:
			ans = super_block->next_available_blk;
			//printf("next_available_blk = %d\n", ans);

			break;
		case 5:
			ans = super_block->blk_size;
			//printf("blk_size = %d\n", ans);
			break;

	}
	close(fd);

	if(ans==-1){
		//printf("(Shell) getSuperBlock(): Invalid Flags\n");
	}

	//printf("\n\n");

	return ans;
}

void setSuperBlock(int flags, int value){


	//printf(">>>OverWrite Super Block: %d<<<\n",flags);

	int fd=open(file_name, O_RDONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	lseek(fd, SB_OFFSET, SEEK_SET);


	struct superblock *super_block = (struct superblock*)malloc(sizeof(struct superblock));

	read(fd, (struct superblock *)super_block, sizeof(struct superblock));


	switch (flags){
		case 0:
			super_block->inode_offset = value;
			//printf("inode_offset = %d\n", value);
			break;
		case 1:
			super_block->max_inode = value;
			//printf("max_inode = %d\n", value);
			break;
		case 2:
			super_block->max_data_blk = value;
			//printf("max_data_blk = %d\n", value);
			break;
		case 3:
			super_block->next_available_inode = value;
			//printf("next_available_inode = %d\n", value);
			break;
		case 4:
			super_block->next_available_blk = value;
			//printf("next_available_blk = %d\n", value);

			break;
		case 5:
			super_block->blk_size = value;
			//printf("blk_size = %d\n", value);
			break;
	}
	close(fd);

	fd=open(file_name, O_WRONLY, 0660);

	lseek(fd, SB_OFFSET, SEEK_SET);	// 4kb for boot

	write(fd, (void *)super_block, sizeof (struct superblock));
	close(fd);

	//printf("\n\n");

}

struct inode *read_inode(int pos){
	//printf("(Read Inode: %d)\n",pos);

	if(pos<0){
		printf("Error!\n");
		return NULL;
	}

	int fd=open(file_name, O_RDONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	lseek(fd, INODE_OFFSET+pos*sizeof(struct inode), SEEK_SET);


	struct inode *new_node = (struct inode*)malloc(sizeof(struct inode));
	read(fd, (struct inode *)new_node, sizeof(struct inode));
	close(fd);

	//printf("i_number = %d\n", new_node->i_number);
	//printf("i_mtime = %lld\n", new_node->i_mtime);
	//printf("i_type = %d\n", new_node->i_type);
	//printf("i_size  = %d\n", new_node->i_size);
	//printf("direct_blk[0] = %d\n", new_node->direct_blk[0]);
	//printf("direct_blk[1] = %d\n", new_node->direct_blk[1]);
	//printf("indirect_blk = %d\n", new_node->indirect_blk);
	//printf("file_num = %d\n", new_node->file_num);
	//printf("\n\n");

	return new_node;

}

void write_inode(int pos, struct inode *new_node){
	//printf("(Write Inode: %d)\n",pos);

	//printf("i_number = %d\n", new_node->i_number);
	//printf("i_mtime = %lld\n", new_node->i_mtime);
	//printf("i_type = %d\n", new_node->i_type);
	//printf("i_size  = %d\n", new_node->i_size);
	//printf("direct_blk[0] = %d\n", new_node->direct_blk[0]);
	//printf("direct_blk[1] = %d\n", new_node->direct_blk[1]);
	//printf("indirect_blk = %d\n", new_node->indirect_blk);
	//printf("file_num = %d\n", new_node->file_num);
	//printf("\n\n");

	int fd=open(file_name, O_WRONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	lseek(fd, INODE_OFFSET+pos*sizeof(struct inode), SEEK_SET);

	write(fd, (void *)new_node, sizeof (struct inode));
	close(fd);

}

struct dir_mapping *read_block(int pos){
	//printf("(Read Directory Mapping: %d)\n",pos);


	int fd=open(file_name, O_RDONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	lseek(fd, DATA_OFFSET+pos*BLOCK_SIZE, SEEK_SET);

	struct dir_mapping *new_map = (struct dir_mapping*)malloc(sizeof(struct dir_mapping));
	read(fd, (struct dir_mapping *)new_map, sizeof(struct dir_mapping));
	close(fd);

	//printf("dir = %s\n", new_map->dir);
	//printf("inode_number = %d\n", new_map->inode_number);
	//printf("\n\n");

	return new_map;

}


void write_block(int pos, struct dir_mapping *new_map){
	//printf("(Write Directory Mapping: %d)\n",pos);

	//printf("dir = %s\n", new_map->dir);
	//printf("inode_number = %d\n", new_map->inode_number);

	int fd=open(file_name, O_WRONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	lseek(fd, DATA_OFFSET+pos*BLOCK_SIZE, SEEK_SET);

	write(fd, (void *)new_map, sizeof (struct dir_mapping));
	close(fd);
	//printf("\n\n");

}

void update_par_list(struct dir_mapping *old_blk,struct dir_mapping *new_blk){
	//printf("(Update Parent List)\n");

	int fd=open(file_name, O_WRONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	//printf("Parent : %s\n", old_blk->dir);
	int pos_b = old_blk->inode_number;

	struct inode *old_node = read_inode(pos_b);

	int pos = old_node->indirect_blk;
	int file_num = old_node->file_num;

	lseek(fd, DATA_OFFSET+pos*BLOCK_SIZE+file_num*sizeof(struct dir_mapping), SEEK_SET);
	 write(fd, (void *)new_blk, sizeof(struct dir_mapping));

	close(fd);

	old_node->file_num = file_num+1;
	write_inode(old_node->i_number,old_node);

}


int parent_inode(int cur){
	struct inode *cur_node = read_inode(cur);
	struct dir_mapping * par_blk = read_block(cur_node->direct_blk[1]);
	return par_blk->inode_number;
}

int get_child_pos(int blk_pos,char* dirname, int max_num){
	//printf("(Read block(%d) Child List:  %s)\n",blk_pos,dirname);


	int fd=open(file_name, O_RDONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	lseek(fd, DATA_OFFSET+blk_pos*BLOCK_SIZE, SEEK_SET);

	struct dir_mapping *tmp_blk = (struct dir_mapping *)malloc (sizeof(struct dir_mapping));
	int j = 0;
	int ans = -1;


	for(j = 0;j<max_num;j++){

		//printf("j = %d\n",j);

		read(fd, (struct dir_mapping *)tmp_blk, sizeof(struct dir_mapping));
		if(strcmp(tmp_blk->dir,dirname)==0){
			ans = tmp_blk->inode_number;
			break;
		}
	}
	close(fd);

	//printf("Child' inode_number = %d\n", ans);
	//printf("\n\n");

	return ans;
}

void get_child_list(int blk_pos, int max_num){
	//printf("(Read block(%d) Child List Display)\n",blk_pos);


	int fd=open(file_name, O_RDONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	lseek(fd, DATA_OFFSET+blk_pos*BLOCK_SIZE, SEEK_SET);

	struct dir_mapping *tmp_blk = (struct dir_mapping *)malloc (sizeof(struct dir_mapping));
	int j = 0;

	struct inode *tmp_node;

	printf("Name\tInode#\tCreate Time\tType\t\tSize(Byte)\n");

	for(j = 0;j<max_num;j++){

		////printf("j = %d\n",j);
		read(fd, (struct dir_mapping *)tmp_blk, sizeof(struct dir_mapping));
		tmp_node = read_inode(tmp_blk->inode_number);
		printf("%s\t%d\t%lld\t",tmp_blk->dir,
			tmp_node->i_number,tmp_node->i_mtime);
		if(tmp_node->i_type==1){
			printf("Regular File\t");
		}else{
			printf("Directory\t");
		}
		printf("%d\t\n",tmp_node->i_size);
	}
	close(fd);
}

int child_node(int cur, char* dirname){

	//printf("child_node: %d %s\n",cur,dirname);


	if(strcmp(dirname,"..")==0){
		return parent_inode(cur);
	}else if(strcmp(dirname,".")==0){
		return cur;
	}

	struct inode *cur_node = read_inode(cur);
	int blk_pos = cur_node->indirect_blk;

	int child_pos = get_child_pos(blk_pos,dirname,cur_node->file_num);


	return child_pos;
}

int get_dir_node(int par_blk, char* dir_name){
	struct dir_mapping *old_blk = read_block(par_blk);
	struct inode *old_node = read_inode(old_blk->inode_number);

	int nxt_inode = get_child_pos(old_node->indirect_blk,dir_name,old_node->file_num);

	return nxt_inode;
}

int create_dir(int par_blk, char* dir_name, int flags){

	//printf("Create a Directory: %s in block(%d)\n",dir_name,par_blk);
	//printf("=========================================\n");

	int nxt_inode = 0;
	int nxt_block = 0;

	if(flags == 0){
		nxt_inode = getSuperBlock(3);
		nxt_block = getSuperBlock(4);
	}else{
		printf("Warn: Replace with a Directory\n");
		nxt_inode = get_dir_node(par_blk, dir_name);
		struct inode *tmp_node = read_inode(nxt_inode);
		nxt_block = tmp_node->direct_blk[0];
	}


	struct inode *new_node=(struct inode *)malloc (sizeof(struct inode));
	struct dir_mapping *new_map=(struct dir_mapping *)malloc (sizeof(struct dir_mapping));

	new_node->i_number = nxt_inode;
	new_node->i_mtime = time(NULL);
	new_node->i_type = 0;
	new_node->i_size = 0;
	new_node->direct_blk[0] = nxt_block;	// self content
	new_node->direct_blk[1] = par_blk;	// parent content
	new_node->indirect_blk = nxt_block+1;	// child list
	new_node->file_num = 0;

	strcpy(new_map->dir, dir_name);

	new_map->inode_number = nxt_inode;

	write_inode(nxt_inode,new_node);
	write_block(nxt_block,new_map);

	nxt_block+=2;

	if(flags==0){
		nxt_inode++;
		setSuperBlock(3,nxt_inode);
		setSuperBlock(4,nxt_block);
		if(par_blk!=-1){	// not root directory
			struct dir_mapping *old_blk = read_block(par_blk);
			update_par_list(old_blk,new_map);
		}
	}

	//printf("\n\n");

	return new_node->i_number;
}


int create_file(int par_blk, char* dir_name, int flags){

	//printf("Create a File: %s in block(%d)\n",dir_name,par_blk);
	//printf("=========================================\n");

	int nxt_inode = 0;
	int nxt_block = 0;

	if(flags == 0){
		nxt_inode = getSuperBlock(3);
		nxt_block = getSuperBlock(4);
	}else{
		printf("Warn: Replace with a Regular File\n");
		nxt_inode = get_dir_node(par_blk, dir_name);
		struct inode *tmp_node = read_inode(nxt_inode);
		nxt_block = tmp_node->direct_blk[0];
	}


	struct inode *new_node=(struct inode *)malloc (sizeof(struct inode));
	struct dir_mapping *new_map=(struct dir_mapping *)malloc (sizeof(struct dir_mapping));

	new_node->i_number = nxt_inode;
	new_node->i_mtime = time(NULL);
	new_node->i_type = 1;					// file
	new_node->i_size = 0;
	new_node->direct_blk[0] = nxt_block;	// self content
	new_node->direct_blk[1] = par_blk;		// parent content
	new_node->indirect_blk = nxt_block+1;	// child list
	new_node->file_num = 0;

	strcpy(new_map->dir, dir_name);

	new_map->inode_number = nxt_inode;

	write_inode(nxt_inode,new_node);
	write_block(nxt_block,new_map);


	update_content_list(new_node->indirect_blk,0,0,nxt_block+11);

	nxt_block+=12;

	if(flags==0){
		nxt_inode++;
		setSuperBlock(3,nxt_inode);
		setSuperBlock(4,nxt_block);
		if(par_blk!=-1){	// not root directory
			struct dir_mapping *old_blk = read_block(par_blk);
			update_par_list(old_blk,new_map);
		}
	}



	//printf("\n\n");

	return new_node->i_number;

}

void update_content_list(int old_blk, int offset, int size,int new_blk){
	//printf("(Update File Content List)\n");

	//printf("Block List: %d, offset: %d\n",old_blk,offset);

	int fd=open(file_name, O_WRONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	struct file_blk *new_file_blk = (struct file_blk *)malloc (sizeof(struct file_blk));

	new_file_blk->size = size;
	new_file_blk->blk_pos = new_blk;

	//printf("file_blk->size = %d\n",new_file_blk->size);
	//printf("file_blk->blk_pos = %d\n",new_file_blk->blk_pos);
	//printf("\n\n");

	lseek(fd, DATA_OFFSET+old_blk*BLOCK_SIZE+offset*sizeof(struct file_blk), SEEK_SET);
	write(fd, (void *)new_file_blk, sizeof(struct file_blk));

	close(fd);
}


struct file_blk *read_content_list(int old_blk, int offset){
	//printf("(Read File Content List)\n");

	//printf("Block List: %d, offset: %d\n",old_blk,offset);

	int fd=open(file_name, O_RDONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	struct file_blk *new_file_blk = (struct file_blk *)malloc (sizeof(struct file_blk));




	lseek(fd, DATA_OFFSET+old_blk*BLOCK_SIZE+offset*sizeof(struct file_blk), SEEK_SET);
	read(fd, (void *)new_file_blk, sizeof(struct file_blk));

	//printf("file_blk->size = %d\n",new_file_blk->size);
	//printf("file_blk->blk_pos = %d\n",new_file_blk->blk_pos);
	//printf("\n\n");

	close(fd);

	return new_file_blk;
}

void get_file_content(int blk,int offset,void *buff, int count){

	//printf("Get File Content: block:%d,  offset:%d, count:%d\n",blk,offset,count);

	// if(offset>1000||count>1000){
	// 	//printf("wrong\n");
	// 	return;
	// }


	int fd=open(file_name, O_RDONLY, 0660);

	lseek(fd, DATA_OFFSET+BLOCK_SIZE*blk+offset, SEEK_SET);

	if(!fd)
		printf("Open File Error\n");

	read(fd, buff, count);



	close(fd);
}

void set_file_content(int blk,int offset,void *buff, int count){

	//printf("Set File Content: block:%d,  offset:%d, count:%d\n",blk,offset,count);

	// if(offset>1000||count>1000){
	// 	//printf("wrong\n");
	// 	return;
	// }

	int fd=open(file_name, O_WRONLY, 0660);

	lseek(fd, DATA_OFFSET+BLOCK_SIZE*blk+offset, SEEK_SET);

	if(!fd)
		printf("Open File Error\n");


	write(fd, buff, count);

	close(fd);
}

struct path_ans *path_analysis(char* path,int nxt_dir){

	//printf("(Path Analysis)\n");

	char tmp_dir[10];
	char *p = path;

	//nxt_dir = 0;
	int pre_dir = -1;
	int err = 0;

	struct inode* tmp_node;

	int path_len = strlen(path);
	if(path[path_len-1]!='/'){
		path[path_len] = '/';
		path[path_len+1] = 0;
	}


	//printf("%s\n\n",path);

	int i = 0;
	while(*p){

		if(*p=='/'){

			tmp_dir[i] = 0;


			if(nxt_dir==-1){
				err = 1;
				break;
			}

			if(pre_dir!=-1){
				tmp_node = read_inode(pre_dir);
				if(tmp_node->i_type!=0){
					err = 1;
					break;
				}
			}

			if(i==0){

				if(pre_dir == -1){
					pre_dir = 0;
					nxt_dir = 0;
					//printf("->root\n");
				}else{
					err = 1;
					break;
				}

			}else if(strcmp(tmp_dir, ".")==0){

				pre_dir = nxt_dir;
				//printf("->self\n");

			}else if(strcmp(tmp_dir, "..")==0){

				pre_dir = nxt_dir;
				nxt_dir = parent_inode(nxt_dir);
				//printf("->parent\n");

			}else{

				pre_dir = nxt_dir;
				nxt_dir = child_node(nxt_dir,tmp_dir);
				//printf("->%s\n",tmp_dir);
			}
			i = 0;
			//printf("\n");

		}else{
			tmp_dir[i++] = *p;
		}
		p++;
	}

	if(err == 1){
		//printf("Wrong Path!\n");
		return NULL;
	}else{
	 struct path_ans *ans=(struct path_ans *)malloc (sizeof(struct path_ans));

	 if(nxt_dir==0){
	 	ans->target_inode = -1;
	 }else{
		ans->target_inode = pre_dir;
	 }

	 strcpy(ans->target_name,tmp_dir);
	 ans->isExist = 1;
	 if(nxt_dir==-1)
	 	ans->isExist = 0;

	// printf("\n");
	// printf("target_inode = %d\n", ans->target_inode);
	// printf("target_name = %s\n", ans->target_name);
	// printf("isExist = %d\n", ans->isExist);
	// printf("\n\n");

	 return ans;
	}
}


void init_superblock(){

	//printf("Init Super Block:\n");
	//printf("=========================================\n");

	int fd=open(file_name, O_WRONLY, 0660);

	if(!fd)
		printf("Open File Error\n");


	lseek(fd, SB_OFFSET, SEEK_SET);

	struct superblock *super_block = (struct superblock*)malloc(sizeof(struct superblock));


	super_block->inode_offset = INODE_OFFSET;	// 8 KB
	super_block->data_offset = DATA_OFFSET;	// 8 MB
	super_block->max_inode = MAX_INODE;
	super_block->max_data_blk = MAX_DATA_BLK;
	super_block->next_available_inode = 0;
	super_block->next_available_blk = 10;
	super_block->blk_size = BLOCK_SIZE;		// 4 KB

	// printf("inode_offset = %d\n", super_block->inode_offset);
	//printf("data_offset = %d\n", super_block->data_offset);
	//printf("max_inode = %d\n", super_block->max_inode);
	//printf("max_data_blk = %d\n", super_block->max_data_blk);
	//printf("next_available_inode = %d\n", super_block->next_available_inode);
	//printf("next_available_blk = %d\n", super_block->next_available_blk);
	//printf("blk_size = %d\n", super_block->blk_size);

	write(fd, (void *)super_block, sizeof (struct superblock));
	close(fd);
}

void read_real_file(char* path, int inode){

	//printf("Read a Real File: %s,  node:%d\n",path, inode);

	int fd=open(path, O_RDONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	int offset = 0;
	int count = 0;
	byte buff[8192];

	while(1){
		count = read(fd, buff, sizeof(buff));

		//printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		//printf("offset = %d, count = %d\n",offset,count);
		//printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

		if(count<=0) break;

		write_t(inode,offset,buff,count);
		offset += count;

	}

	//printf("End of Reading....\n\n\n\n");

	close(fd);

}

void write_real_file(char* path, int inode){
	//printf("Write a Real File: %s,  node:%d\n",path, inode);

	int create =open(path, O_CREAT, 0660);
	close(create);

	int fd=open(path, O_WRONLY, 0660);

	if(!fd)
		printf("Open File Error\n");

	int offset = 0;
	int count = 0;
	byte buff[8192];

	while(1){
		count = read_t(inode,offset,buff,sizeof(buff));

		//printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		//printf("offset = %d, count = %d\n",offset,count);
		//printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

		if(count<=0) break;

		write(fd, buff, count);
		offset += count;

	}

	close(fd);
}

void inside_copy(int sou, int des){
	//printf("Copy Inode %d to Inode %d\n",sou, des);

	int offset = 0;
	int count = 0;
	byte buff[8192];

	while(1){
		count = read_t(sou,offset,buff,sizeof(buff));

		//printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
		//printf("offset = %d, count = %d\n",offset,count);
		//printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

		if(count<=0) break;

		write_t(des,offset,buff,count);
		offset += count;

	}

}


int init_the_file_system(char *drive_name){

	file_name = drive_name;

	printf("Init Super Block\n");
	init_superblock();

	printf("Create Root Directory\n");
	create_dir(-1,"root",0);

	return 0;
}
