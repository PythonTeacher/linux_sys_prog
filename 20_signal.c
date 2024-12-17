#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void signal_handler(int signum) {
    printf("%d signal received\n", signum);
}

int main(int argc, char **argv) {
    signal(SIGTERM, signal_handler);    // 핸들러 호출
    signal(SIGINT, SIG_IGN);            // Ctrl+C 시그널 무시
    signal(SIGKILL, SIG_IGN);           // KILL은 무시 불가
    while(1) {
        printf("loop...\n");
        sleep(1);
    }
    return 0;
}