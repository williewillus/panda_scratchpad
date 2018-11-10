#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <immintrin.h>

#define CACHELINE 64
int main()
{
    /* Variable to store user content */
    char* data = "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR";
    char* data_over = "SSSSSSSSSSSSSS";
    
    int i;
    //char temp_buf[CACHELINE+1];
    char* temp_buf;
    temp_buf = (char*)malloc(sizeof(char)*CACHELINE);
    memset(temp_buf, 'A', CACHELINE);
    //for (i=0; i<CACHELINE; i++)
    //            temp_buf[i] = 'A';
    //temp_buf[i]='\0';
    int fd;


    fd = open("/mnt/pmem0/write.txt", O_WRONLY| O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
	perror("Cannot open file");
	exit(1);
    }

    int num_blocks, res;
    /*for (num_blocks=0; num_blocks < 1; num_blocks ++)
    	res = write(fd, temp_buf, CACHELINE);
    
    if (fsync(fd) < 0) {
   	perror("Error fsync");
   	exit(1);
    }*/

    _mm_sfence();
 
    //lseek(fd, 0, SEEK_SET);    
    res = write(fd, data, strlen(data));
    
    if (fsync(fd) < 0) {
   	perror("Error fsync");
   	exit(1);
    }

    printf("File written and fsynced - data = %d\n", res);
    close(fd);

    return 0;
}
