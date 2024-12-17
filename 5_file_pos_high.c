#include <stdio.h>
#include <string.h>

int write_to_file() {
	FILE *fp;

	fp = fopen("data.txt", "w");
	if (fp == NULL) {
		printf("fopen failed\n");
		return -1;
	}

	printf("offset1 = %ld\n", ftell(fp));
	fputs("hello\n", fp);
	printf("offset2 = %ld\n", ftell(fp));
	fputs("world\n", fp);
	printf("offset3 = %ld\n", ftell(fp));

	fclose(fp);
	return 0;
}

int read_from_file() {
	FILE *fp;
	char buf[1024];

	fp = fopen("data.txt", "r+");
	if (fp == NULL) {
		printf("fopen failed\n");
		return -1;
	}

	printf("offset4 = %ld\n", ftell(fp));
	memset(buf, 0, sizeof(buf));
	
	fgets(buf, sizeof(buf), fp);		// hello\n
	printf("%s", buf);
	printf("offset5 = %ld\n", ftell(fp));
	
	fseek(fp, 0, SEEK_END);
	printf("offset6 = %ld\n", ftell(fp));
	fputs("final\n", fp);
	printf("offset7 = %ld\n", ftell(fp));

	fclose(fp);
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