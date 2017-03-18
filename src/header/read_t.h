int read_t(int node_number, int offset, void *buff,int count){

	if(node_number<0){
		printf("Wrong Inode Number\n");
		return -1;
	}

	if(offset<0||count<0){
		printf("Wrong Parameter\n");
		return -1;
	}

	//printf("Read File: Inode(%d) Offset(%d) %d bytes\n", node_number,offset,count);
	//printf("=========================================\n");

	struct inode *file_node = read_inode(node_number);

	int con_blk = file_node->indirect_blk;


	int i = 0;

	int file_b = file_node->i_size/BLOCK_SIZE;

	int begin_b = offset/BLOCK_SIZE;

	int end_b = (offset+count)/BLOCK_SIZE;

	struct file_blk *tmp_file_blk;

	int tmp_off,tmp_count;

	int tmp_size = 0;

	byte *pw = buff;

	int origin_count = count;

	int real_count = 0;

	i = begin_b;

	while(1){

		//printf("\n>>>proecess node %d\n",i);
		//printf("----------------------------------------\n");

		if(i>file_b){
			break;
		}


		tmp_file_blk = read_content_list(con_blk,i);

		//printf("file_blk->size = %d\n",tmp_file_blk->size);
		//printf("file_blk->blk_pos = %d\n",tmp_file_blk->blk_pos);


		if(i==begin_b){
			tmp_off = offset%BLOCK_SIZE;
		}else if(i>begin_b){
			tmp_off = 0;
		}

		if(tmp_off+count>tmp_file_blk->size){
			tmp_count = tmp_file_blk->size-tmp_off;
		}else{
			tmp_count = count;
		}

		if(i==file_b){
			if(tmp_off>=tmp_file_blk->size){
				//printf("offset > File End\n");
				return 0;
			}
		}


		get_file_content(tmp_file_blk->blk_pos,tmp_off,pw,tmp_count);
		pw+=tmp_count;


		real_count += tmp_count;


		if(i>=end_b){

			// if(offset+origin_count>file_node->i_size){

			// }else{

			// }

			break;
		}

		count-=tmp_count;

		i++;

	}

	return real_count;


}
