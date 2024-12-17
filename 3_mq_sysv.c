#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define IPC_KEY_FILENAME    "/proc"         // 읽을 수 있는 파일
#define IPC_KEY_PROJ_ID     1234            // int

struct msgbuf {
    long mtype;
    char mtext[1024];
};

int get_msq_id() {
    key_t key;
    int msq_id;

    // key_t ftok(const char *pathname, int proj_id);
    key = ftok(IPC_KEY_FILENAME, IPC_KEY_PROJ_ID);
    printf("key: %x\n", key);
    if (key == -1) {
        perror("ftok() error");
        return -1;
    }

    // int msgget(key_t key, int msgflg);
    msq_id = msgget(key, 0644 | IPC_CREAT);
    printf("msq_id: %d\n", msq_id);
    if(msq_id == -1) {
        perror("msgget() error");
        return -1;
    }
    return msq_id;
}

int do_send() {
    struct msgbuf mbuf;
    int msq_id;
    int ret;

    msq_id = get_msq_id();
    if (msq_id == -1) {
        perror("get_msq_id() error");
        return -1;
    }

    // int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
    memset(&mbuf, 0, sizeof(mbuf));
    mbuf.mtype = 1;     // 양수로 지정
    strncpy(mbuf.mtext, "hello world", sizeof(mbuf.mtext) - 1);
    ret = msgsnd(msq_id, &mbuf, sizeof(mbuf.mtext), 0);
    if (ret == -1) {
        perror("msgsnd() error");
        return -1;
    }

    return 0;
}

int do_recv() {
    struct msgbuf mbuf;
    int msq_id;
    int ret;

    msq_id = get_msq_id();
    if (msq_id == -1) {
        perror("get_msq_id() error");
        return -1;
    }
    
    // ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
    memset(&mbuf, 0, sizeof(mbuf));
    // 0: 첫번째 메시지 수신
    // 양수: msgtype에 매치되는 첫번째 메시지 수신
    // 음수 : 절대값보다 작거나 같은 msgtype에 매치되는 첫번째 메시지 수신 
    ret = msgrcv(msq_id, &mbuf, sizeof(mbuf.mtext), 1, 0); 
    if (ret == -1) {
        perror("msgrcv() error");
        return -1;
    }
    printf("received message: %s, mtype: %ld\n", mbuf.mtext, mbuf.mtype);
    
    return 0;
}

int main(int argc, char **argv) {
    int ret;

    // ./mq_sysv s
    // ./mq_sysv r
    if (argc < 2) {
        printf("Usage: %s (s or r)\n", argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "s")) {
        ret = do_send();
    } else if (!strcmp(argv[1], "r")) {
        ret = do_recv();
    } else {
        printf("Usage: %s (s or r)\n", argv[0]);
        return -1;
    }
    return ret;
}