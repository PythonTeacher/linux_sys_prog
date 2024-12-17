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

// 세마포어 잠금 함수
int sem_lock(int semid) {
    // sem_op
    // - 양수: semaphore 해제
    // - 0: semaphore 값이 0이 될 때까지 기다림(block) 
    // - 음수: semaphore 획득
    // struct sembuf sb = {0, -1, 0};  
    struct sembuf sb;
    sb.sem_num = 0;             // n번째 세마포어 (0부터 시작)
    sb.sem_op = -1;             // 세마포어 감소 연산 (P 연산)
    sb.sem_flg = SEM_UNDO;      // 프로세스 종료 시 동작 취소
    if (semop(semid, &sb, 1) == -1) {
        perror("semop lock");
        return -1;
    }
    return 0;
}

// 세마포어 해제 함수
int sem_unlock(int semid) {
    // struct sembuf sb = {0, 1, 0};   // 세마포어 증가 연산 (V 연산)
    struct sembuf sb;
    sb.sem_num = 0;             // n번째 세마포어 (0부터 시작)
    sb.sem_op = 1;              // 세마포어 증가 연산 (V 연산)
    sb.sem_flg = SEM_UNDO;      // 프로세스 종료 시 동작 취소
    if (semop(semid, &sb, 1) == -1) {
        perror("semop unlock");
        return -1;
    }
    return 0;
}

// child process
int do_add(struct shared_data *info, int semid) {
    int i;

    // 세마포어 잠금
    sem_lock(semid);

    // shared memory에 데이터 쓰기
    for(i = 0; i < MAX_TRY; i++) {
        info->counter++;
        printf("child: counter %d\n", info->counter);
    }

    // 세마포어 잠금 해제
    sem_unlock(semid);

    return 0;
}

// parent process
int do_sub(struct shared_data *info, int semid) {
    int i;

    // 세마포어 잠금
    sem_lock(semid);

    // shared memory에 데이터 쓰기
    for(i = 0; i < MAX_TRY; i++) {
        info->counter--;
        printf("parent: counter %d\n", info->counter);
    }    
	
    // 세마포어 잠금 해제
    sem_unlock(semid);

    return 0;
}

// ./shm_sysv_sem
int main() {
    key_t key;
    int shmid, semid;
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

    // 세마포어 생성
    semid = semget(key, 1, 0644 | IPC_CREAT);
    if (semid == -1) {
        perror("semget()");
        return -1;
    }

    // 세마포어 초기화 (처음에는 1로 설정, 즉 사용 가능 상태)
    semctl(semid, 0, SETVAL, 1);	/* binary semaphore */
    // semctl(semid, 1, SETVAL, 3);	/* counting semaphore */

    pid = fork();
    if (pid > 0) {      // parent
        do_sub(info, semid);
        wait(NULL);     // 부모 프로세스 대기

        printf("final counter: %d\n", info->counter);

        // shared memory detach
        if (shmdt(info) == -1) {
            perror("shmdt() error");
            return -1;
        }

        // 리소스 해제 (세마포어와 공유 메모리 제거)
        semctl(semid, 0, IPC_RMID);     // 세마포어 삭제
        shmctl(shmid, IPC_RMID, NULL);  // 공유 메모리 삭제

    } else if (pid == 0) {  // child
        do_add(info, semid);
    } else {
        perror("fork()");
        return -1;
    }

    return 0;
}