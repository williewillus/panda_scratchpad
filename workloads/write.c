#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define PAGESIZE 64
int main()
{
    /* Variable to store user content */
    char* data = "RRRRRRRRRRRRRRR";
    char* data_over = "SSSSSSSSSSSSSS";
    
    int i;
    char temp_buf[PAGESIZE];

    for (i=0; i<PAGESIZE; i++)
                temp_buf[i] = 'A';

    int fd;


    fd = open("/mnt/pmem0/write.txt", O_WRONLY| O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
	perror("Cannot open file");
	exit(1);
    }

    //int res = write(fd, temp_buf, PAGESIZE);

    int res = write(fd, data, strlen(data));
    if (fdatasync(fd) < 0) {
   	perror("Error fsync");
   	exit(1);
    }

    //sync();
    
    printf("File written and fsynced\n");
    close(fd);

    return 0;
}
