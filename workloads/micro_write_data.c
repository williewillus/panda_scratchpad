#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <sched.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/wait.h>

#define FSIZE (5ULL*1024*1024*1024)
#define BLKSIZE size_of_block
#define MBSIZE (1024*1024)
#define KBSIZE (1024)
#define GBSIZE (1024*1024*1024)
#define CLSIZE 64
#define HUGEPAGESIZE (2*1024*1024)
#define PAGESIZE (4096)

char *addr;
struct timeval start,end;
unsigned long long size_of_block;

#define CLFLUSH_SIZE 64

void do_initial_writes(int fd, unsigned long long data_to_be_written) {

	unsigned long long i = 0, num_blocks = 0, offset = 0;
	int extent_number = 0;
	char temp_buf[PAGESIZE];
	double time_taken = 0.0;

	for (i=0; i<PAGESIZE; i++)
		temp_buf[i] = 'A';

        num_blocks = data_to_be_written / (PAGESIZE);
	
	
	for (i = 0 ; i < num_blocks ; i++) {
		lseek(fd, 0, SEEK_END);
                offset += write(fd, temp_buf, PAGESIZE);
	}
	

	if (offset != num_blocks*PAGESIZE) {
		printf("%s: writes failed\n", __func__);
		exit(-1);
	}
	

	printf("################ INITIAL WRITES DONE ##################\n");

	if (fsync(fd) == -1) {
		printf("%s: fsync failed! %s\n", __func__, strerror(errno));
		exit(-1);
	}

	printf("################ INITIAL FSYNC DONE ##################\n");

}


void perform_experiment(int fd, int seq, unsigned long long data, unsigned long long size_of_buf, int is_append, int fsync_freq) {

	char buf[size_of_buf];
	double time_taken = 0.0;
	unsigned long long size_of_write = size_of_buf, i = 0, offset = 0;
	unsigned long long extent_number = 0, offset_block = 0, num_ops = 0;
	unsigned long num_blocks = 0;
	int total_data = 0;	


	num_ops = data / size_of_write;
	total_data = data / KBSIZE;

	printf("%s: fd = %d, seq = %d, num_ops = %llu, size_of_buf = %llu, is_append = %d, total_data = %d KB, fsync_freq = %d\n", __func__, fd, seq, num_ops, size_of_buf, is_append, total_data, fsync_freq);
	fflush(NULL);

	size_of_block = size_of_buf;
	num_blocks = FSIZE / BLKSIZE;
	
	for (i = 0; i < size_of_buf; i++)
		buf[i] = 'R';

	srand(5);

	offset = 0;	
	extent_number = 0;

	for (i = 0 ; i < num_ops ; i++) {

		if (is_append || seq)
			offset_block = i;
		else 
			offset_block = rand() % (num_blocks);
		
		offset = offset_block*size_of_buf;

		//if (pwrite(fd, buf, size_of_buf, offset) != size_of_write ) {
		lseek(fd, offset, SEEK_SET);
		if (write(fd, buf, size_of_buf) != size_of_write ) {
			printf("%s: write failed! %s \n", __func__, strerror(errno));
			exit(-1);
		}

		if (fsync_freq > 0) {
			if ((i + 1) % fsync_freq == 0) {
				if (fsync(fd) != 0) {
					printf("%s: fsync failed, err = %s\n", __func__, strerror(errno));
					exit(-1);
				}

			}
		}
	}

	if (fsync_freq == -1) {
		if (fsync(fd) == -1) {
			printf("%s: fsync failed, err = %s\n", __func__, strerror(errno));
			exit(-1);
		}
	}
}

void setup_arguments(char *argv[], int *seq, unsigned long long *data, unsigned long long *size_of_buf, int *is_append, int *fsync_freq) {

	int granularity;
	unsigned long value;
	char value_str[10];
	int i;
	
	
	if(!strcmp(argv[1], "seq"))
		*seq = 1;
	else
		*seq = 0;

	if(argv[2][strlen(argv[2])-1] == 'K')
		*data = KBSIZE;
	else if(argv[2][strlen(argv[2])-1] == 'M')	       
		*data = MBSIZE;
	else if(argv[2][strlen(argv[2])-1] == 'G')
		*data = GBSIZE;

	strcpy(value_str, argv[2]);
	value_str[strlen(argv[2])-1] = '\0';

	value = atoi(value_str);
	*data = *data * value;

	value_str[0] = '\0';
	
	if(argv[3][strlen(argv[3])-1] == 'K')
		*size_of_buf = KBSIZE;
	else if(argv[3][strlen(argv[3])-1] == 'M')	       
		*size_of_buf = MBSIZE;		
	else
		*size_of_buf = 1;
	
	strcpy(value_str, argv[3]);	
	if(value_str[strlen(argv[3])-1] == 'K' || value_str[strlen(argv[3])-1] == 'M')
	   value_str[strlen(argv[3])-1] = '\0';

	value = atoi(value_str);
	*size_of_buf = *size_of_buf * value;

	if (!strcmp(argv[4], "append")) 
		*is_append = 1;
	else
		*is_append = 0;

	value_str[0] = '\0';
	strcpy(value_str, argv[5]);
	*fsync_freq = atoi(value_str);
}

int main(int argc, char *argv[])
{
	int fd = -1, seq = 0, is_append = 0, fsync_freq = 0;
	unsigned long long data = 0, size_of_buf = 0;

	if(argc != 6) {
		printf("Usage: ./a.out <seq/rand> <data> <granularity_of_write> <append/overwrite> <fsync_freq (eg: -1 -> end fsync 0 -> no fsync, 1 -> 1 in every write, 10 -> 1 in every 10 writes, etc)>\n");
		exit(-1);
	}

	setup_arguments(argv, &seq, &data, &size_of_buf, &is_append, &fsync_freq);
	
	printf("################ STARTING HERE ##################\n");
	//creating new file and allocating it in the beginning

	fd = open("/mnt/pmem0/writerand.txt", O_RDWR | O_CREAT);
	if(fd < 0) {
		printf("%s: file not opened. err = %s\n", __func__, strerror(errno));
		exit(-1);
	}

	printf("################ FILE OPENED ##################\n");

	if (is_append == 0)
		do_initial_writes(fd, PAGESIZE);

	close(fd);
	
	fd = open("/mnt/pmem0/writerand.txt", O_RDWR);
	if(fd < 0) {
		perror("file not opened!\n");
		exit(-1);
	}
	
//-------------------------------------------------------------------------------------
	
	//printf("################ STARTING MAIN WORKLOAD ##################\n");

	//perform_experiment(fd, seq, data, size_of_buf, is_append, fsync_freq);

//-------------------------------------------------------------------------------------

    	close(fd);
	return 0;
}
