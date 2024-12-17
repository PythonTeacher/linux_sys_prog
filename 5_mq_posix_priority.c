// mq_posix.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

// virtual file system에 mount됨 (man mq_overview에 나옴)
#define POSIX_MQ_NAME   "/posixmq"

int init_mq() {
    mqd_t mqd;
    struct mq_attr attr;

    /* mqd_t mq_open(const char *name, int oflag);
       mqd_t mq_open(const char *name, int oflag, mode_t mode,
                     struct mq_attr *attr); */
    // NULL로 설정 시 시스템 Defualt값으로 설정
    mqd = mq_open(POSIX_MQ_NAME, O_RDWR | O_CREAT, 0644, NULL);

    if (mqd == -1) {
        perror("mq_open() error");
        return -1;
    }

    // MQ 속성값 확인하기
    // int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
    memset(&attr, 0, sizeof(attr));
    if (mq_getattr(mqd, &attr) == -1) {
        close(mqd);
        return -1;
    }

    printf("mq_flags : %ld\n", attr.mq_flags);
    printf("mq_maxmsg : %ld\n", attr.mq_maxmsg);
    printf("mq_msgsize : %ld\n", attr.mq_msgsize);
    printf("mq_curmsgs : %ld\n", attr.mq_curmsgs);

    return mqd;
}

int do_send(unsigned int priority) {
    mqd_t mqd;
    char buf[100];
    int ret;

    mqd = init_mq();
    if (mqd == -1) {
        perror("init_mq() error");
        return -1;
    }

    // int mq_send(mqd_t mqdes, const char *msg_ptr,
    //                  size_t msg_len, unsigned int msg_prio);
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "hello world, pid %d", getpid());

    ret = mq_send(mqd, buf, strlen(buf), priority);
    if (ret == -1) {
        perror("mq_send() error");
        close(mqd);
        return -1;
    }
    
    if (mq_close(mqd) == -1) {
       perror("mq_close() error");
       return -1; 
    }  

    return 0;
}

int do_recv() {    
    mqd_t mqd;
    char buf[8192];    
    int ret;
    unsigned int priority;

    mqd = init_mq();
    if (mqd == -1) {
        perror("init_mq() error");
        return -1;
    }

// ssize_t mq_receive(mqd_t mqdes, char *msg_ptr,
//                    size_t msg_len, unsigned int *msg_prio);

    memset(buf, 0, sizeof(buf));

    ret = mq_receive(mqd, buf, sizeof(buf), &priority);
    if (ret == -1) {
        perror("mq_receive() error");
        close(mqd);
        return -1;
    }
    printf("received message: %s, priority: %u\n", buf, priority);

    if (mq_close(mqd) == -1) {
       perror("mq_close() error");
       return -1; 
    }  
    
    return 0;
}

// ./mq_posix_priority s 10
// ./mq_posix_priority r
int main(int argc, char **argv) {
    int ret;
    unsigned int priority;

    if (argc < 2) {
        printf("Usage: %s (s or r)\n", argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "s")) {
        priority = strtoul(argv[2], NULL, 10);
        ret = do_send(priority);
    } else if (!strcmp(argv[1], "r")) {
        ret = do_recv();
    } else {
        printf("Usage: %s (s or r)\n", argv[0]);
        return -1;
    }

    return ret;
}