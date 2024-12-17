#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

int main(int argc, char **argv) {

    int fd;
    char *file = NULL;
    struct stat statbuf;

    if (argc < 2) {
        printf("Usage: %s filename\n", argv[0]);
        return -1;
    }

    if ((fd = open(argv[1], O_RDONLY)) < 0) {
        perror("open error");
        return -1;
    }

    if (fstat(fd, &statbuf) < 0) {
        perror("fstat error");
        close(fd);
        return -1;
    }

    printf("size: %ld\n", statbuf.st_size);
    printf("page size: %ld\n", sysconf(_SC_PAGESIZE));

    file = (char *) mmap(0, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file == MAP_FAILED) {
        perror("mmap erorr");
        close(fd);
        return -1;
    }

    printf("%s", file);
    printf("%s", file+6);
    
    if (munmap(file, statbuf.st_size) == -1) {
        perror("munmap error");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}