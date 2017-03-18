int open_t(const char *pathname, int flags){

	//printf("Open File: %s\n", pathname);
	//printf("=========================================\n");

	struct path_ans *path_info;

	char tmp_path[80];

	strcpy(tmp_path,pathname);

	path_info = path_analysis(tmp_path,cur);

	int ans = -1;

	struct inode *tmp_node = read_inode(path_info->target_inode);

	if(tmp_node==NULL) return ans;

	if(path_info->target_inode==-1){
		printf("Wrong Path\n");
		flags = 5;
	}

	if(flags==0){		// create a file
		ans = create_file(tmp_node->direct_blk[0],path_info->target_name,path_info->isExist);
	}else if(flags==1){ // create a directory
		ans = create_dir(tmp_node->direct_blk[0],path_info->target_name,path_info->isExist);
	}else if(flags == 2){
		ans = get_dir_node(tmp_node->direct_blk[0], path_info->target_name);
	}else{
		printf("Illegal Flags\n");
	}

	//printf("Result: inode %d\n",ans);

	//printf("\n\n");

	return ans;
}
