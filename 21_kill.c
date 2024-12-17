#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    pid_t pid;
    int signum;

    if (argc < 3) {
        printf("Usage: %s PID SIGNUM\n", argv[0]);
        return -1;
    }
    pid = atoi(argv[1]);
    signum = atoi(argv[2]);

    kill(pid, signum);       // 2: SIGINT, 9: SIGKILL, 15: SIGTERM
    printf("%d signal sent!!\n", signum);

    return 0;
}
