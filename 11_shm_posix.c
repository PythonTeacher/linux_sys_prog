#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h> 

#define POSIX_SHM_NAME   "/posixshm"

struct shared_data {
	int pid;
	int counter;
};

int do_write() {
    int fd;
    struct shared_data *info;

    // shared memory object 생성
    fd = shm_open(POSIX_SHM_NAME, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("shm_open()");
        return -1;
    }

    // shared memory 크기 설정
    if (ftruncate(fd, sizeof(struct shared_data)) == -1) {
	perror("ftruncate()");
	close(fd);
	return -1;
    }

    // shared memory mapping
    info = mmap(0, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (info == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    close(fd);

    // shared memory에 데이터 쓰기
    info->pid = getpid();
    info->counter++;

    printf("writer: pid %d, counter %d\n", info->pid, info->counter);
	
    // shared memory unmap
    if (munmap(info, sizeof(struct shared_data)) == -1) {
        perror("munmap");
        return -1;
    }
    
    // remove shared memory object
    //shm_unlink(fd);

    return 0;
}

int do_read() {
    int fd;
    struct shared_data *info;

    // shared memory object 생성
    fd = shm_open(POSIX_SHM_NAME, O_RDONLY, 0);
    if (fd == -1) {
        perror("shm_open()");
        return -1;
    }

    // shared memory mapping
    info = mmap(0, sizeof(struct shared_data), PROT_READ, MAP_SHARED, fd, 0);
    if (info == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    close(fd);

    // shared memory에서 데이터 읽기
    printf("reader: pid %d, counter %d\n", info->pid, info->counter);
	
    // shared memory unmap
    if (munmap(info, sizeof(struct shared_data)) == -1) {
        perror("munmap");
        return -1;
    }
 
    // remove shared memory object
    //shm_unlink(fd);

    return 0;
}

// ./shm_posix write
// ./shm_posix read
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

