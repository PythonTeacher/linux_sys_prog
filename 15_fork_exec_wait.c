#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void print_pid(int x) {
    printf("parent pid : %d, current pid : %d, x : %d\n", 
            getppid(), getpid(), x);
}

int main(int argc, char **argv) {
    pid_t pid;
    int x = 0;
    int exit_status;

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

    pid = wait(&exit_status);       // 자식프로세스 종료 대기
    if (WIFEXITED(exit_status)) {   // 정상 종료인 경우
        printf("child pid : %d exited, exit status : %d\n",
                pid, WEXITSTATUS(exit_status));
    }

    if (WIFSIGNALED(exit_status)) { // 시그널에 의해 종료된 경우 (kill -9로 자식프로세스 강제종료시키기)
        printf("child pid : %d exited, signal : %d\n",
                pid, WTERMSIG(exit_status));
    }

    return 0;
}

-------------------------------------------
< exec_test2.c >

#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    printf("exec_test2.c pid: %d\n", getpid());
    int i;

    sleep(5);		// 5초 동안 대기

    for(i = 0; i < argc; i++) {
        printf("argv%d => %s\n", i, argv[i]);
    }
    return 1;		// status code 1
}
