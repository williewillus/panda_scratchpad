#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/ioctl.h>
#include <asm/unistd.h>
#include <inttypes.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <assert.h>
#include <time.h>
//#include <emmintrin.h>


#define FSIZE (5ULL*1024*1024*1024)
#define MMAP_SIZE (1024*1024*512)
#define BLKSIZE (4096)
#define MBSIZE (1024*1024)
#define KBSIZE (1024)
#define GBSIZE (1024*1024*1024)
#define CLSIZE 64
#define HUGEPAGESIZE (2*1024*1024)

struct timeval start,end;
char pageFaultCommand[100];
char *addr = NULL;

#define CLFLUSH_SIZE 64

#define _mm_clflush(addr)\
	asm volatile("clflush (%0)" :: "r"((volatile char *)addr));
	//asm volatile(".byte 0x66; clflush %0" : "+m" (*(volatile char *)addr));



void setupPageFaultCommand(char *statCommand) {

	char procpath[100];
	int myPid;
	char strMyPid[10];

	strcpy(procpath, "/proc/");
	myPid = getpid();
	sprintf(strMyPid, "%d", myPid);
	strcat(procpath, strMyPid);
	strcat(procpath, "/stat");

	strcpy(statCommand, "cut -d \" \" -f ");
	strcat(statCommand, strMyPid);
	strcat(statCommand, ",2,10,12 ");
	strcat(statCommand, procpath);

}


static inline void do_cflushopt_len(volatile void* addr, size_t length)
{
	// note: it's necessary to do an mfence before and after calling this function
	size_t i;
	for (i = 0; i < length; i += CLFLUSH_SIZE) {
		//printf("\nTrying to do a clflush");
		_mm_clflush((void *)(addr + i));
	}

	//perfmodel_add_delay(0, length);
}

void doInitialWrites(int fd, unsigned long long dataToBeWritten) {

	unsigned long long i;
	int extentNumber = 0;
	char tempbuf[BLKSIZE];
	unsigned long long offset = 0;
	double time_taken;
	unsigned long long num_blocks;

	for(i=0; i<BLKSIZE; i++)
		tempbuf[i] = 'A';

        num_blocks = dataToBeWritten / (BLKSIZE);
	
	
	for(i = 0 ; i < num_blocks ; i++)
        {
		lseek(fd, 0, SEEK_END);
                offset += write(fd, tempbuf, BLKSIZE);
	}
	

	if(offset != num_blocks*BLKSIZE) {
		printf("%s: writes failed\n", __func__);
		exit(-1);
	}
	

	printf("################ INITIAL WRITES DONE ##################\n");

	if(fsync(fd) == -1) {
		printf("%s: fsync failed! %s\n", __func__, strerror(errno));
		exit(-1);
	}

	printf("################ INITIAL FSYNC DONE ##################\n");

}

void performExperiment(int fd, int seq, unsigned long long numOps, unsigned long long sizeOfBuf) {

	char buf[sizeOfBuf];
	char buf2[sizeOfBuf];
	char buf3[sizeOfBuf];
	char buf4[sizeOfBuf];
	
	double timeTaken;
	unsigned long long size_of_write = sizeOfBuf;
	unsigned long long i,j;
	unsigned long long offset;
	double time_taken;
	unsigned long sizeOfBlock;
	unsigned long long numBlocks = 0;
	unsigned long long extentNumber = 0;
	unsigned long long offsetBlock;

	numBlocks = numOps;

	printf("%s: fd = %d, seq = %d, numOps = %llu, sizeOfBuf = %llu\n", __func__, fd, seq, numOps, sizeOfBuf);
	fflush(NULL);

	for(i = 0; i < sizeOfBuf; i++)
		buf[i] = 'R';
	
	srand(5);

	offset = 0;
	
	//do_ioctl_call(PERF_EVENT_IOC_ENABLE);

	extentNumber = 0;

	for (i = 0; i < 1; i++) {

		offsetBlock = i;					
		offset = offsetBlock*BLKSIZE;

		do_cflushopt_len((void *)(addr + offset), (BLKSIZE));		
	}
	
	//system(pageFaultCommand);

	for(i = 0 ; i < numBlocks ; i++)
	{
		offset = i*sizeOfBuf;

		/*
		if( memmove_nodrain_movnt_64(addr + offset, buf, sizeOfBuf) == NULL ) { 
			printf("%s: memcpy failed! \n", __func__);
		}
		*/
		
		for (j = 0; j < 1; j++) {
			//pmdk_memmove_temporal(addr + offset, buf, sizeOfBuf);

			/*
			if( memmove_nodrain_movnt_64(addr + offset, buf, sizeOfBuf) == NULL ) { 
				printf("%s: memcpy failed! \n", __func__);
			}
			*/

			/*
			if (memcpy(buf2, addr + offset, sizeOfBuf) == NULL) {
				printf("%s: memcpy_failed\n", __func__);
			}
			*/

			if (memcpy(addr, buf, sizeOfBuf) == NULL) {
				printf("%s: memcpy failed\n", __func__);
			}
			//emulate_latency_ns(125);
		}

		//perfmodel_add_delay(0, sizeOfBuf);

		do_cflushopt_len(addr + offset, sizeOfBuf);

	}
}

void performMmap(int fd) {
	
	char tempbuf[BLKSIZE];
	int i;

	for (i=0; i<BLKSIZE; i++) {

		tempbuf[i] = 'B';
	}

	unsigned long long offset = 0;

		
	if((addr = (char *) mmap(NULL, BLKSIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_SHARED, fd, 0)) == MAP_FAILED) {
		perror("mmap failed!");
		exit(-1);
	}
	//16777216

	while(offset <= BLKSIZE) {
		
		if(memcpy(addr, tempbuf, BLKSIZE) == NULL) {
			printf("%s: mmap memcpy failed\n", __func__);
			exit(-1);
		}

		offset += BLKSIZE;

	}

}

void setupArguments(char *argv[], int *seq, unsigned long long *numOps, unsigned long long *sizeOfBuf) {

	int granularity;
	unsigned long value;
	char valueStr[10];
	int i;
	
	
	if(!strcmp(argv[1], "seq"))
		*seq = 1;
	else
		*seq = 0;

	if(argv[2][strlen(argv[2])-1] == 'K')
		*numOps = KBSIZE;
	else if(argv[2][strlen(argv[2])-1] == 'M')	       
		*numOps = MBSIZE;
	else if(argv[2][strlen(argv[2])-1] == 'G')
		*numOps = GBSIZE;

	strcpy(valueStr, argv[2]);
	valueStr[strlen(argv[2])] = '\0';

	value = atoi(valueStr);
	*numOps = value;

	valueStr[0] = '\0';
	
	if(argv[3][strlen(argv[3])-1] == 'K')
		*sizeOfBuf = KBSIZE;
	else if(argv[3][strlen(argv[3])-1] == 'M')	       
		*sizeOfBuf = MBSIZE;		
	else
		*sizeOfBuf = 1;
	
	strcpy(valueStr, argv[3]);	
	if(valueStr[strlen(argv[3])-1] == 'K' || valueStr[strlen(argv[3])-1] == 'M')
	   valueStr[strlen(argv[3])-1] = '\0';

	value = atoi(valueStr);
	*sizeOfBuf = *sizeOfBuf * value;
}

int main(int argc, char *argv[])
{
	int fd;
	int seq;
	unsigned long numBlocks, numLinesInEachBlock;
	long long cache_misses;
	unsigned long long numOps, sizeOfBuf;
	int huge;

	if(argc != 4) {
		printf("Usage: ./a.out seq/rand <numOps> <granularityOfWrite>\n");
		exit(-1);
	}

	//setupPageFaultCommand(pageFaultCommand);
	setupArguments(argv, &seq, &numOps, &sizeOfBuf);
	
	printf("################ STARTING HERE ##################\n");
	//creating new file and allocating it in the beginning

	fd = open("/mnt/pmem0/writerand.txt", O_RDWR | O_CREAT, 0666);
	if(fd < 0) {
		perror("file not opened!\n");
		exit(-1);
	}

	printf("################ FILE OPENED ##################\n");

	doInitialWrites(fd, BLKSIZE);

	close(fd);
	
	fd = open("/mnt/pmem0/writerand.txt", O_RDWR);
	if(fd < 0) {
		perror("file not opened!\n");
		exit(-1);
	}
	
//-------------------------------------------------------------------------------------
	
	//system("echo 3 > /proc/sys/vm/drop_caches");

	//printf("################ STARTING MAIN WORKLOAD ##################\n");

	performMmap(fd);
	
	//performExperiment(fd, seq, numOps, sizeOfBuf);

//-------------------------------------------------------------------------------------

    	close(fd);
	return 0;
}
