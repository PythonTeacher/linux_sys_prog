#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FIFO_FILE   "./fifofile"
char buf[1024];

int do_write() {
    int fd;

    // 파이프 파일이 있으면 mkfifo 실패
    // 파일 삭제를 위해 unlink() 호출
    unlink(FIFO_FILE);

    printf("before mkfifo\n");
    if (mkfifo(FIFO_FILE, 0644)) {
        perror("mkfifo() error");
        return -1;
    }

    printf("before write open\n");
    fd = open(FIFO_FILE, O_WRONLY);
    if (fd < 0) {
        perror("open() error");
        return -1;
    }

    write(fd, buf, strlen(buf));
    close(fd);

    return 0;
}

int do_read() {
    int fd;
    

    printf("before read open\n");
    fd = open(FIFO_FILE, O_RDONLY);
    if (fd < 0) {
        perror("open() error");
        return -1;
    }

    read(fd, buf, sizeof(buf));
    printf("received message => %s\n", buf);
    close(fd);

    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s (w or r)\n", argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "w")) {
        /* writer */
	strncpy(buf, argv[2], strlen(argv[2]));
        do_write();
    } else if (!strcmp(argv[1], "r")) {
        /* reader */
        do_read();
    }
    return 0;
}