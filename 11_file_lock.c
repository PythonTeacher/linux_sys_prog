#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>

int main(int argc, char **argv) {
    int fd;
    int op;     // LOCK_SH, LOCK_EX

    if (argc < 2) {
        printf("Usage: %s operation(sh | ex)\n", argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "sh")) {
        op = LOCK_SH;
    } else if (!strcmp(argv[1], "ex")) {
        op = LOCK_EX;
    } else {
        printf("Usage: %s operation(sh | ex)\n", argv[0]);
        return -1;
    }

    if ((fd = open("lockfile", O_RDWR | O_CREAT, 0644)) < 0) {
        perror("open error");
        return -1;
    }

    printf("trying to grab file lock..\n");
    if (flock(fd, op)) {
        perror("flock error");
        close(fd);
        return -1;
    }
    printf("grabbed the lock!!\n");
    getc(stdin);

    if (flock(fd, LOCK_UN)) {
        perror("unlock error");
        close(fd);
        return -1;
    }

    printf("unlocked\n");
    close(fd);
    return 0;
}