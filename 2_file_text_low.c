#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int append_file(void)
{
	int fd;

	fd = open("datafile.txt", O_WRONLY | O_APPEND, 0600);
	if (fd == -1) {
		printf("open() error\n");
		return -1;
	}

	char *str = "hello again..\n";

	int result = write(fd, str, strlen(str));
	if (result == -1) {
		printf("write error");
		return -1;
	}

	close(fd);
	return 0;
}

int trunc_file(void)
{
	int fd;

	fd = open("datafile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1) {
		printf("open() error\n");
		return -1;
	}

	dprintf(fd, "byebye file\n");
	close(fd);
	return 0;
}

int write_file(void)
{
	int fd;

	fd = open("datafile.txt", O_WRONLY | O_CREAT, 0600);
	if (fd == -1) {
		printf("open() error\n");
		return -1;
	}

	char *str = "hello world!!\n";

	int result = write(fd, str, strlen(str));
	if (result == -1) {
		printf("write error");
		return -1;
	}
	close(fd);
	return 0;
}

int read_file(void)
{
	int fd;
	char buf[1024];

	fd = open("datafile.txt", O_RDONLY);
	if (fd == -1) {
		printf("open error\n");
		return -1;
	}

	memset(buf, 0, sizeof(buf));
	int result = read(fd, buf, sizeof(buf));
	if (result == -1) {
		printf("read error");
		return -1;
	}

	printf("%s\n", buf);
	close(fd);

	return 0;
}


int main(int argc, char **argv)
{
	if (write_file()) {
		printf("write_file() error\n");
		return -1;
	}
	if (read_file()) {
		printf("read_file() error\n");
		return -1;
	}
	if (append_file()) {
		printf("append_file() error\n");
		return -1;
	}
/*
	if (trunc_file()) {
		printf("trunc_file() error\n");
		return -1;
	}
	if (append_file()) {
		printf("append_file() error\n");
		return -1;
	}
*/
	return 0;
}

