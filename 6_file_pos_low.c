#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int write_to_file() {
	int fd;

	fd = open("data2.txt", O_WRONLY | O_CREAT, 0644);
	if (fd == -1) {
		printf("open failed\n");
		return -1;
	}

	printf("offset1 = %ld\n", lseek(fd, 0, SEEK_CUR));
	dprintf(fd, "hello\n");
	printf("offset2 = %ld\n", lseek(fd, 0, SEEK_CUR));
	dprintf(fd, "world\n");
	printf("offset3 = %ld\n", lseek(fd, 0, SEEK_CUR));

	close(fd);
	return 0;
}

int read_from_file() {
	int fd;
	char buf[1024];

	fd = open("data2.txt", O_RDWR);
	if (fd == -1) {
		printf("open failed\n");
		return -1;
	}

	printf("offset4 = %ld\n", lseek(fd, 0, SEEK_CUR));
	memset(buf, 0, sizeof(buf));
	
	read(fd, buf, sizeof(buf));		// hello\nworld\n
	printf("%s", buf);
	printf("offset5 = %ld\n", lseek(fd, 0, SEEK_CUR));
	
	lseek(fd, 0, SEEK_END);
	printf("offset6 = %ld\n", lseek(fd, 0, SEEK_CUR));
	dprintf(fd, "final\n");
	printf("offset7 = %ld\n", lseek(fd, 0, SEEK_CUR));

	close(fd);
	return 0;
}

int main(int argc, char **argv) {
	if (write_to_file()) {
		printf("write_to_file() failed\n");
		return -1;
	}

	if (read_from_file()) {
		printf("read_from_file() failed\n");
		return -1;
	}

	return 0;
}