#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct person {
    char name[20];
    int age;
};

int write_to_file() {
    int fd;
    ssize_t ret;

    fd = open("person2.dat", O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        printf("open failed\n");
        return -1;
    }
    
    struct person p = {        
        .name = "IU",
        .age = 29
    };

    ret = write(fd, &p, sizeof(struct person));
    if (ret == -1) {
        printf("write failed\n");
        close(fd);
        return -1;
    } else if (ret != sizeof(struct person)) {
        printf("writed failed(partial write)\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int read_from_file() {
    int fd;
    ssize_t ret;
    struct person p;

    fd = open("person2.dat", O_RDONLY);
    if (fd == -1) {
        printf("open failed\n");
        return -1;
    }

    ret = read(fd, &p, sizeof(struct person));
    if (ret == -1) {
        printf("read failed\n");
        close(fd);
        return -1;
    } else if (ret != sizeof(struct person)) {
        printf("read failed(partial read)\n");
        close(fd);
        return -1;
    }

    printf("name:%s, age:%d\n", p.name, p.age);

    close(fd);
    return 0;
}

int main(int argc, char **argv) {

    if (write_to_file()) {
        printf("write_to_file failed\n");
        return -1;
    }

    if (read_from_file()) {
        printf("read_from _file failed\n");
        return -1;
    }

    return 0;
}