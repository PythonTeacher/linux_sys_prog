#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    pid_t pid;
    union sigval value;

    if (argc < 3 ) {
        printf("Usage: %s PID PAYLOAD\n", argv[0]);
        return -1;
    }

    pid = atoi(argv[1]);
    value.sival_int = atoi(argv[2]);   // int값을 보내는 것은 되지만 다른 프로세스로
                                              // 주소값을 보내는 것은 안됨 (IPC 사용으로 하기)

    sigqueue(pid, SIGUSR1, value);  // 10(사용자정의 시그널)
    return 0;
}
