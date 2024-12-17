#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// void * 타입으로 리턴해주기
// 전달값이 없으면 void *arg는 생략 가능
void *child_thread_main(void *arg) {
    int i;
    pthread_t t_id;

    t_id = pthread_self();
    char *name = (char *)arg;

    printf("child thread id %ld started [%s]\n", t_id, name);

    for (i = 100; i >= 0; i -= 10) {
 	printf("[%s] => %d\n", (char *)arg, i);
        sleep(1);
    }
    printf("child thread id %ld ended [%s]\n", t_id, name);
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t child_threads[2];
    int ret;
	
    printf("main thread id %ld\n", pthread_self());    // 호출 스레드 ID(main thread)

    ret = pthread_create(&child_threads[0], NULL, child_thread_main, "runner1");
    if (ret) {
	perror("pthread_create error");
	return -1;
    }
    ret = pthread_create(&child_threads[1], NULL, child_thread_main, "runner2");
    if (ret) {
	perror("pthread_create error");
	return -1;
    }

    sleep(12);
    printf("main thread ended!!\n");

    return 0;
}
