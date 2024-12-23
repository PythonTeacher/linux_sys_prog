#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

struct shared_data {
	int pid;
	int counter;
};

struct shared_data *do_mapping() {
	struct shared_data *info;

	info = mmap(NULL, sizeof(struct shared_data), 
			    PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if (info == MAP_FAILED) {
		perror("mmap()");
		return NULL;
	}

	// 0으로 memset 초기화하지 않아도 됨
	printf("init: pid->%d, counter->%d\n", info->pid, info->counter);
	return info;
}


// child process
int do_write(struct shared_data *info) {
	int n;
	
	n = 0;
	while (n < 5) {
		info->pid = getpid();
		info->counter++;

	    	printf("writer: pid %d, counter %d\n", info->pid, info->counter);
		n++;
		sleep(1);
	}

	munmap(info, sizeof(struct shared_data));

	return 0;
}

// parent process
int do_read(struct shared_data *info) {
	int n;

	n = 0;
	while (n > 5) {
       		printf("reader: pid %d, counter %d\n", info->pid, info->counter);
		n++;
		sleep(1);
	}
    
	munmap(info, sizeof(struct shared_data));

	return 0;
}

// ./anony_mapping
// fork() -> chlid에서 write(), parent에서 read
int main(int argc, char **argv)
{
	struct shared_data *info;
	int pid;

	info = do_mapping();
	if (!info) {
		perror("do_mapping()");
		return -1;
	}

	pid = fork();
	if (pid > 0) {			// parent
		usleep(500000);		// microsecond (1000000 -> 1초)
		do_read(info);
	} else if (pid == 0) {	// child
		do_write(info);
		return 0;
	} else {
		perror("fork()");
		return -1;
	}

	return 0;
}