#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#define CACHELINE 4096
int main()
{
    /* Variable to store user content */
    char* data = "RRRRRRRRRRRRRRR";
    char* data_over = "SSSSSSSSSSSSSS";
    
    int i;
    char temp_buf[CACHELINE];

    for (i=0; i<CACHELINE; i++)
                temp_buf[i] = 'R';

    int fd;


    fd = open("/mnt/pmem0/write.txt", O_WRONLY| O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
	printf("Cannot open file");
	exit(1);
    }

    int res = write(fd, temp_buf, CACHELINE);

    //int res = write(fd, data, strlen(data));
    if (fsync(fd) < 0) {
   	printf("Error fsync");
   	exit(1);
    }
    
    printf("File written and fsynced\n");
    close(fd);

    return 0;
}
