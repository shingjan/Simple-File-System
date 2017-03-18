int write_t(int node_number, int offset, void *buff,int count){

	//printf("Write File: Inode(%d) Offset(%d) %d bytes\n", node_number,offset,count);
	//printf("=========================================\n");
	if(node_number<0){
		printf("Wrong Inode Number\n");
		return -1;
	}

	if(offset<0||count<0){
		printf("Wrong Parameter\n");
		return -1;
	}

	struct inode *file_node = read_inode(node_number);

	int con_blk = file_node->indirect_blk;


	int i = 0;

	int file_b = file_node->i_size/BLOCK_SIZE;

	int begin_b = offset/BLOCK_SIZE;

	int end_b = (offset+count)/BLOCK_SIZE;

	struct file_blk *tmp_file_blk = (struct file_blk *)malloc (sizeof(struct file_blk));

	int tmp_off,tmp_count;

	int tmp_size = 0;

	byte *pw = buff;

	int origin_count = count;

	int real_count = 0;

	if(begin_b>=file_b){
		i = file_b;
	}


	while(1){

		//printf("\n>>>proecess node %d\n",i);
		//printf("----------------------------------------\n");

		if(i==file_b){
			tmp_file_blk = read_content_list(con_blk,i);

			tmp_file_blk->size = BLOCK_SIZE;

			update_content_list(con_blk, i, BLOCK_SIZE,tmp_file_blk->blk_pos);
		}else if(i>file_b){
			int nxt_blk = getSuperBlock(4);
			update_content_list(con_blk, i, BLOCK_SIZE,nxt_blk);
			setSuperBlock(4,nxt_blk+1);
			tmp_file_blk = read_content_list(con_blk,i);
		}else{
			tmp_file_blk = read_content_list(con_blk,i);
		}

		//printf("file_blk->size = %d\n",tmp_file_blk->size);
		//printf("file_blk->blk_pos = %d\n",tmp_file_blk->blk_pos);



		if(i==begin_b){
			tmp_off = offset%BLOCK_SIZE;
		}else if(i>begin_b){
			tmp_off = 0;
		}else{
			tmp_off = -1;
		}

		if(tmp_off+count>BLOCK_SIZE){
			tmp_count = BLOCK_SIZE-tmp_off;
		}else{
			tmp_count = count;
		}


		if(tmp_off!=-1){
			set_file_content(tmp_file_blk->blk_pos,tmp_off,pw,tmp_count);
			pw+=tmp_count;

			real_count += tmp_count;

		}


		if(i>=end_b){

			if(i>=file_b){
				if(offset+count>file_node->i_size){
					tmp_size = (offset+count) % BLOCK_SIZE;
				}else{
					tmp_size = (file_node->i_size) % BLOCK_SIZE;
				}
				update_content_list(con_blk, i, tmp_size,tmp_file_blk->blk_pos);
			}


			if(offset+origin_count>file_node->i_size){
				tmp_size = (offset+origin_count);
			}else{
				tmp_size = (file_node->i_size);
			}

			file_node->i_size = tmp_size;
			write_inode(file_node->i_number,file_node);

			break;
		}

		if(tmp_off!=-1)
			count -= tmp_count;

		i++;

	}

	return real_count;

}
