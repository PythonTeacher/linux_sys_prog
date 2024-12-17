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

        //if (execl("/bin/ls", "ls", "-al", NULL) == -1) {
        if (execl("./exec_test2", "exec_test2", "hello", "world", NULL) == -1) {
            perror("execl error");
            return -1;
        }        
    } else{
        perror("fork error");
        return -1;
    }

    getc(stdin);
    return 0;
}