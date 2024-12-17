#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define SHARED_FILENAME	"shared_file"

struct shared_data {
	int pid;
	int counter;
};

int do_write() {
	int fd;
	struct shared_data *info;
	int n;

	fd = open(SHARED_FILENAME, O_RDWR | O_CREAT, 0644);     // Read, Write 둘다 함
	if (fd == -1) {
		perror("open()");
		return -1;
	}

    	// shared memory 크기 설정 (안하면 코어덤프 오류 발생)
    	// file size가 0이므로 8바이트가 되도록 사이즈를 늘려줘야 함
	if (ftruncate(fd, sizeof(struct shared_data)) == -1) {
		perror("ftruncate()");
		close(fd);
		return -1;
	}

	info = mmap(NULL, sizeof(struct shared_data), 
			    PROT_WRITE, MAP_SHARED, fd, 0);
   	printf("write: %p\n", info);
	if (info == MAP_FAILED) {
		perror("mmap()");
		close(fd);
		return -1;
	}

	close(fd);

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

int do_read() {
	int fd;
	struct shared_data *info;
	int n;

	fd = open(SHARED_FILENAME, O_RDONLY);
	if (fd == -1) {
		perror("open()");
		return -1;
	}

	info = mmap(NULL, sizeof(struct shared_data), 
			    PROT_READ, MAP_SHARED, fd, 0);
    	printf("read: %p\n", info);
	if (info == MAP_FAILED) {
		perror("mmap()");
		close(fd);
		return -1;
	}

    	close(fd);

	n = 0;
	while (n < 5) {
        	printf("reader: pid %d, counter %d\n", info->pid, info->counter);
		n++;
		sleep(1);
	}
    
	munmap(info, sizeof(struct shared_data));

	return 0;
}

// ./file_mem_mapping write
// ./file_mem_mappig read
int main(int argc, char **argv)
{
	if (argc < 2) {
       		printf("Usage: %s (write or read)\n", argv[0]);
        	return -1;
    	}

    	if (!strcmp(argv[1], "write")) {
        	do_write();
    	} else if(!strcmp(argv[1], "read")) {
        	do_read();
    	} else {
        	printf("Usage: %s (write or read)\n", argv[0]);
        	return -1;        
    	}
	return 0;
}

