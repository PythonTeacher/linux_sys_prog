< exec_test.c >

#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    printf("exec_test.c pid: %d\n", getpid());

    // argv[0], argv[1], argv[2], NULL 포인터로 끝내주기
    execl("./exec_test2", "exec_test2", "hello", "world", NULL);

    //char *args[] = {"exec_test2", "hello", "world", NULL};
    //execv("./exec_test2", args);

    printf("bye!!\n");      // 출력안됨
    return 0;
}

< exec_test2.c >

#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]){
    printf("exec_test2.c pid: %d\n", getpid());
    int i;

    for(i = 0; i < argc; i++) {
        printf("argv%d => %s\n", i, argv[i]);
    }
    return 0;
}