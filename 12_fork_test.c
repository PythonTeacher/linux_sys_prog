#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int x = 0;

    fork();

    x = 1;
    printf("pid : %d, x : %d\n", getpid(), x);
    getc(stdin);
    return 0;
}

=> 터미널1에서 실행 후 터미널2에서 ps -ef --forest로 확인
----------------------------------------------------------------------

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

void print_pid(int x) {
    printf("parent pid : %d, current pid : %d, x : %d\n", 
            getppid(), getpid(), x);
}

int main(int argc, char **argv) {
    pid_t pid;
    int x = 0;

    pid = fork();
    printf("fork() 후 pid : %d\n", pid);

    if (pid > 0) {  // 부모 프로세스
        printf("I'm a parent process\n");
        x = 1;
        print_pid(x);
    } else if (pid == 0) {    // 자식 프로세스
        printf("I'm a child process\n");
        x = 2;
        print_pid(x);
    } else{
        perror("fork error");
        return -1;
    }

    getc(stdin);
    return 0;
}

=> 터미널1에서 실행 후 터미널2에서 ps -ef --forest로 확인