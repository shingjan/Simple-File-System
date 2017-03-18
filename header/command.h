void make_file(char* path)
{

    pid_t childpid;
    pid_t waitR = 0;
    int status;
    childpid = fork();

    if (childpid == 0) {

        //printf("Child Process: %d\n",getpid());

        open_t(path, 0);
        exit(0);
    }
    else if (childpid > 0) {
        //printf("Parent Process: %d\n",getpid());

        waitR = wait(&status);
        //printf("Collect Child Process\n");
    }
}

void make_dir(char* path)
{

    pid_t childpid;
    pid_t waitR = 0;
    int status;


    //printf("I am process %d\n",getpid());
    //printf("before fork\n");
    childpid = fork();

    if (childpid == 0) {
        //printf("Child Process: %d\n",getpid());

        open_t(path, 1);
        exit(0);
    }
    else if (childpid > 0) {
        //printf("Parent Process: %d\n",getpid());

        waitR = wait(&status);
        //printf("Collect Child Process\n");
    }
}

void cd_path(char* path)
{
    struct path_ans* path_info;
    struct inode* tmp_node;
    int tmp = 0;
    path_info = path_analysis(path, cur);

    if (path_info == NULL) {
        printf("Wong Path\n");
    }else if (path_info->isExist == 0) {
        printf("Wong Path\n");
    }else {
        if (path_info->target_inode == -1) {
            path_info->target_inode = cur;
        }

        tmp = child_node(path_info->target_inode, path_info->target_name);
        //printf("%d\n",tmp);
        tmp_node = read_inode(tmp);
            if (tmp_node->i_type == 0) {
                cur = tmp_node->i_number;
            }else{
            	printf("It is a Regular File\n");
            }

    }
}

void ls_command()
{
    pid_t childpid;
    pid_t waitR = 0;
    int status;
    struct inode* tmp_node;
    int file_num = 0;
    int tmp_blk = 0;

    childpid = fork();

    if (childpid == 0) {
        //printf("Child Process: %d\n",getpid());

        tmp_node = read_inode(cur);
        tmp_blk = tmp_node->indirect_blk;
        file_num = tmp_node->file_num;

        printf("\n\n-------------------------\n");
        get_child_list(tmp_blk, file_num);
        printf("\n-------------------------\n\n");


        //printf("End of Child Process\n");

        exit(0);
    }
    else if (childpid > 0) {
        //printf("Parent Process: %d\n",getpid());

        waitR = wait(&status);
        //printf("Collect Child Process\n");
    }
}

void cat(char* path)
{
    pid_t childpid;
    pid_t waitR = 0;
    int status;
    struct inode* tmp_node;
    int tmp = 0;
    int count = 0;
    int offset = 0;
    char buff[1024];
    childpid = fork();

    if (childpid == 0) {
        //printf("Child Process: %d\n",getpid());

        tmp = open_t(path, 2);

        if(tmp!=-1)
	        tmp_node = read_inode(tmp);

        if(tmp==-1){
        	printf("File not Exists\n");
        }else if (tmp_node->i_type == 1) { // is a file
            printf("\n\n-------------------------\n");
            while (1) {
                count = read_t(tmp, offset, (char*)buff, sizeof(buff));
                if (count == 0)
                    break;
                offset += count;
                printf("%s", buff);
            }
            printf("\n-------------------------\n\n");
        }
        else {
            printf("Can not cat directory\n");
        }
        //printf("End of Child Process\n");

        exit(0);
    }
    else if (childpid > 0) {
        //printf("Parent Process: %d\n",getpid());

        waitR = wait(&status);
        //printf("Collect Child Process\n");
    }
}

void external(char* path, char* path1)
{
    pid_t childpid;
    pid_t waitR = 0;
    int status;
    int tmp = 0;

    childpid = fork();

    if (childpid == 0) {
        //printf("Child Process: %d\n",getpid());

        tmp = open_t(path, 0);

        if(tmp<0){
        	printf("Error!");
        }else{
        	read_real_file(path1, tmp);
        }


        //printf("End of Child Process\n");

        exit(0);
    }
    else if (childpid > 0) {
        //printf("Parent Process: %d\n",getpid());

        waitR = wait(&status);
        //printf("Collect Child Process\n");
    }
}

void internal(char* path, char* path1)
{
    pid_t childpid;
    pid_t waitR = 0;
    int status;
    int tmp = 0;
    struct inode *tmp_node;

    childpid = fork();

    if (childpid == 0) {
        //printf("Child Process: %d\n",getpid());

        tmp = open_t(path, 2);


		tmp_node = read_inode(tmp);
		if(tmp_node->i_type==0){
			tmp = -1;
		}

        if(tmp<0){
        	printf("Error!\n");
        }else{
	        write_real_file(path1, tmp);
	    }
        //printf("End of Child Process\n");

        exit(0);
    }
    else if (childpid > 0) {
        //printf("Parent Process: %d\n",getpid());

        waitR = wait(&status);
        //printf("Collect Child Process\n");
    }
}

void inner_copy(char* path, char* path1)
{
    pid_t childpid;
    pid_t waitR = 0;
    int status;
    int des = 0;
    int tmp = 0;
    struct inode *tmp_node;
    childpid = fork();

    if (childpid == 0) {
        //printf("Child Process: %d\n",getpid());

        tmp = open_t(path, 2);
        des = open_t(path1, 0);

        tmp_node = read_inode(tmp);
		if(tmp_node->i_type==0){
			tmp = -1;
		}

		if(tmp==-1){
			printf("Error!\n");
		}else{
			inside_copy(tmp, des);
		}


        //printf("End of Child Process\n");

        exit(0);
    }
    else if (childpid > 0) {
        //printf("Parent Process: %d\n",getpid());

        waitR = wait(&status);
        //printf("Collect Child Process\n");
    }
}
