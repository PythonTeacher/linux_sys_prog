#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#define TARGET_FILE	"hello"

int main(int argc, char **argv) {

	// Hardlink 생성
	if (link(TARGET_FILE, "hello_link")) {
		printf("link() failed : %d\n", errno);
       		perror("error msg : ");
		return -1;
	}

	// Softlink 생성
	if (symlink(TARGET_FILE, "hello_symlink")) {
		perror("error msg : ");
		return -1;
	}

	return 0;
}