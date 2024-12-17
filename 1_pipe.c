#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    int pipe_fds[2];
    pid_t pid;
    char buf[1024];
    int wstatus;
    
    // 3-5반 추가하기
    memset(buf, 0, sizeof(buf));

    if (pipe(pipe_fds)) {
        perror("pipe() error");
        return -1;
    }

    pid = fork();
    if (pid > 0) {
        /* parent process (write) */
        close(pipe_fds[0]);     // reader fd close
        strncpy(buf, "hello, child!", sizeof(buf) - 1);
        write(pipe_fds[1], buf, strlen(buf));
        printf("[%d] sent message : %s\n", getpid(), buf);
        close(pipe_fds[1]);
        
        pid = wait(&wstatus);  // 자식 프로세스 종료 대기
    } else if (pid == 0) {
        /* child process (read) */
        close(pipe_fds[1]);     // writer fd close
        read(pipe_fds[0], buf, sizeof(buf));
        printf("[%d] received message : %s\n", getpid(), buf);
        close(pipe_fds[0]);
    } else {
        /* error */
        perror("fork() error");
        close(pipe_fds[0]);
        close(pipe_fds[1]);
        return -1;
    }
}