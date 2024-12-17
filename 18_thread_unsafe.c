#include <stdio.h>
#include <pthread.h>

#define MAX_TRY		10000000

int shared = 0;

void *child_thread_main(void *arg) {
	int i;
	printf("child pthread id %ld\n", pthread_self());

	for (i = 0; i < MAX_TRY; i++) {
		shared++;
	}
	for (i = 0; i < MAX_TRY; i++) {
		shared--;
	}

	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t child_threads[2];
	int ret;
	
	printf("main pthread id %lu\n", pthread_self());

	ret = pthread_create(&child_threads[0], NULL, child_thread_main, NULL);
	if (ret) {
		perror("pthread_create error");
		return -1;
	}
	ret = pthread_create(&child_threads[1], NULL, child_thread_main, NULL);
	if (ret) {
		perror("pthread_create error");
		return -1;
	}

        printf("trying to join %ld\n", child_threads[0]);
        pthread_join(child_threads[0], NULL);

        printf("trying to join %ld\n", child_threads[1]);
        pthread_join(child_threads[1], NULL);

        printf("main thread ended!!\n");

	printf("shared = %d\n", shared);

	return 0;
}
