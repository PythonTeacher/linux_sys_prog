#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

#define IPC_KEY_FILENAME    "/proc"        // 읽을 수 있는 파일
#define IPC_KEY_PROJ_ID     100            // int
#define MAX_TRY		        100000

// 공유 메모리에 저장할 데이터 구조체 정의
struct shared_data {
    int counter;
};

// child process
int do_add(struct shared_data *info) {
    int i;

    // shared memory에 데이터 쓰기
    for(i = 0; i < MAX_TRY; i++) {
        info->counter++;
        printf("child: counter %d\n", info->counter);
    }

    return 0;
}

// parent process
int do_sub(struct shared_data *info) {
    int i;

    // shared memory에 데이터 쓰기
    for(i = 0; i < MAX_TRY; i++) {
        info->counter--;
        printf("parent: counter %d\n", info->counter);
    }    

    return 0;
}

// ./shm_sysv_unsafe
int main() {
    key_t key;
    int shmid;
    struct shared_data *info;
    int pid;

    // 키 생성
    key = ftok(IPC_KEY_FILENAME, IPC_KEY_PROJ_ID);
    if (key == -1) {
        perror("ftok()");
        return -1;
    }

    // 공유 메모리 생성
    shmid = shmget(key, sizeof(struct shared_data), 0644 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget()");
        return -1;
    }

    // shared memory를 process 주소 공간에 attach
    info = (struct shared_data *)shmat(shmid, NULL, 0);
    if (info == (void *)(-1)) {
        perror("shmat()");
        return -1;
    }

    pid = fork();
    if (pid > 0) {      // parent
        do_sub(info);
        wait(NULL);     // 부모 프로세스 대기

        printf("final counter: %d\n", info->counter);

        // shared memory detach
        if (shmdt(info) == -1) {
            perror("shmdt() error");
            return -1;
        }

        // 리소스 해제 (공유 메모리 제거)
        shmctl(shmid, IPC_RMID, NULL);  // 공유 메모리 삭제

    } else if (pid == 0) {  // child
        do_add(info);
    } else {
        perror("fork()");
        return -1;
    }

    return 0;
}