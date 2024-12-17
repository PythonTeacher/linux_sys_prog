#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <pthread.h>

#define IPC_KEY_FILENAME    "/proc"        // 읽을 수 있는 파일
#define IPC_KEY_PROJ_ID     100            // int
#define MAX_TRY		        100000

// 공유 메모리에 저장할 데이터 구조체 정의
struct shared_data {
    int counter;
};

struct thread_arg {
    int semid;
    struct shared_data *info;
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
    sb.sem_flg = SEM_UNDO;      // 비정상종료 시 동작 취소
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
    sb.sem_flg = SEM_UNDO;      // 비정상종료 시 동작 취소
    if (semop(semid, &sb, 1) == -1) {
        perror("semop unlock");
        return -1;
    }
    return 0;
}

void *do_add(void *arg) {
    int i;
    struct thread_arg *input = (struct thread_arg *)arg;

    // 세마포어 잠금
    sem_lock(input->semid);

    // shared memory에 데이터 쓰기
    for(i = 0; i < MAX_TRY; i++) {
        input->info->counter++;
        printf("child: counter %d\n", input->info->counter);
    }

    // 세마포어 잠금 해제
    sem_unlock(input->semid);

    return NULL;
}

// parent process
void *do_sub(void *arg) {
    int i;
    struct thread_arg *input = (struct thread_arg *)arg;

    // 세마포어 잠금
    sem_lock(input->semid);

    // shared memory에 데이터 쓰기
    for(i = 0; i < MAX_TRY; i++) {
        input->info->counter--;
        printf("parent: counter %d\n", input->info->counter);
    }    
	
    // 세마포어 잠금 해제
    sem_unlock(input->semid);

    return NULL;
}

// ./shm_sysv_sem
int main() {
    key_t key;
    int shmid, semid;
    pthread_t child_thread[2];
    int ret;
    struct shared_data *info;
    struct thread_arg input;

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

    memset(&input, 0, sizeof(input));
    input.info = info;

    // 세마포어 생성
    semid = semget(key, 1, 0644 | IPC_CREAT);
    if (semid == -1) {
        perror("semget()");
        return -1;
    }

    input.semid = semid;

    // 세마포어 초기화 (처음에는 1로 설정, 즉 사용 가능 상태)
    semctl(semid, 0, SETVAL, 1);	/* binary semaphore */
    // semctl(semid, 1, SETVAL, 3);	/* counting semaphore */

    ret = pthread_create(&child_thread[0], NULL, do_sub, &input);
    if (ret) {
        perror("pthread_create()");
        return -1;
    }

    ret = pthread_create(&child_thread[1], NULL, do_add, &input);
    if (ret) {
        perror("pthread_create()");
        return -1;
    }
  
    // 자식 쓰레드 기다리기
    printf("trying to join %ld..\n", child_thread[0]);
    pthread_join(child_thread[0], NULL);

    printf("trying to join %ld..\n", child_thread[1]);
    pthread_join(child_thread[1], NULL);

    printf("main thread ended!!\n");
    printf("final counter: %d\n", info->counter);

    // shared memory detach
    if (shmdt(info) == -1) {
        perror("shmdt() error");
        return -1;
    }

    // 리소스 해제 (세마포어와 공유 메모리 제거)
    semctl(semid, 0, IPC_RMID);     // 세마포어 삭제
    shmctl(shmid, IPC_RMID, NULL);  // 공유 메모리 삭제

    return 0;
}