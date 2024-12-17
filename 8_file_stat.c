#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


int main(int argc, char **argv)
{
	struct stat statbuf;

	if (argc < 2) {
		printf("usage: %s filename\n", argv[0]);
		return -1;
	}

	if (stat(argv[1], &statbuf)) {		// symbolic file일 경우 lstat
		printf("stat() failed\n");
		return -1;
	}

	printf("size: %ld\n", statbuf.st_size);
	if (S_ISDIR(statbuf.st_mode)) {
		printf("Directory\n");
	} else if (S_ISREG(statbuf.st_mode)) {
		printf("Regular file\n");
	} else if (S_ISLNK(statbuf.st_mode)) {
		printf("Symbolic file\n");
	} else {
		printf("???\n");
	}

	return 0;
}
