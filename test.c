#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(){
	int fd = open("sht", O_WRONLY);
	char buffer[4];
	char buffer2[] = "l";
//if not such file for opening, file descriptor will return -1
	int bytes = write(fd, buffer, 4*sizeof(char));
	printf("this is the fd: %d and bytes is: %d \n",fd,bytes);
	printf("%c \n",buffer[0]);
}
