#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void siguser1_handler(int signum, siginfo_t *info, void *ucontext) {
    printf("siguser1_handler started\n");

    // si_code: 시그널 코드(sigqueue()를 통해 받으면 SI_QUEUE가 설정됨)
    // si_int: sigqueue()를 통해 보낸 시그널 payload 
    if (info->si_code & SI_QUEUE) {
        printf("signal from sigqueue(), payload:%d\n", info->si_int);
    } else {
        printf("signal from kill\n");   // kill -SIGUSR1 `pidof signal_recv`로 실행한 경우
    }
}

int main(int argc, char **argv) {
    struct sigaction act;

    memset(&act, 0, sizeof(struct sigaction));
    act.sa_sigaction = siguser1_handler;
    act.sa_flags = SA_SIGINFO;  // sa_sigaction에 설정된 핸들러 호출

    // 10(사용자정의 시그널)을 받음
    if (sigaction(SIGUSR1, &act, NULL)) {
        printf("sigaction failed\n");
        return -1;
    }

    while(1) {
        printf("loop...\n");
        sleep(1);
    }
    return 0;
}