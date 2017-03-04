#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "open2.h"
#include "write_t.h"
#include "read_t.h"


int main(){
	int fd = open_t("sht", O_WRONLY);

}
