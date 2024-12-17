#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH   "sock_file"
#define BUF_SIZE    1024

// socket -> bind -> listen -> accept -> send/recv -> close
int do_server() {
    int sockfd;
    struct sockaddr_un addr;
    int peer;
    char buf[BUF_SIZE];
    int ret;

    //int socket(int domain, int type, int protocol);
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket() error");
        return -1;
    }
    printf("socket() 성공\n");

    // int bind(int sockfd, const struct sockaddr *addr,
    //           socklen_t addrlen);
    // man unix : unix domain socket 도움말    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX; // 반드시 AF_UNIX
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    // 소켓을 닫은 후 바로 재사용할 수 없도록 TIME_WAIT 상태를 가짐
    unlink(SOCK_PATH);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind() error");
        close(sockfd);
        return -1;
    }
    printf("bind() 성공\n");

    // int listen(int sockfd, int backlog);
    if (listen(sockfd, 3) == -1) {
        perror("listen() error");
        close(sockfd);
        return -1;       
    }
    printf("listen()....\n");

    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    if ((peer = accept(sockfd, NULL, NULL)) == -1) {
        perror("accept() error");
        close(sockfd);
        return -1;
    }
    printf("accept() 성공\n");

    memset(buf, 0, sizeof(buf));
    // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
    if ((ret = recv(peer, buf, sizeof(buf), 0)) == -1) {
        perror("recv() error");
        close(sockfd);
        return -1;
    }
    printf("server recv() 성공, ret: %d, message: %s\n", ret, buf);

    char *msg = "hi, client";
    if ((ret = send(peer, msg, strlen(msg), 0)) == -1) {
        perror("send() error");
        close(sockfd);
        return -1;
    }
    printf("sever send() 성공, ret: %d\n", ret);

    close(peer);
    close(sockfd);
    return 0;
}

// socket -> connect -> send -> close
int do_client() {
    int sockfd;
    struct sockaddr_un addr;
    char buf[BUF_SIZE];
    int ret;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket() error");
        return -1;
    }
    printf("client socket() 성공\n");

    // int connect(int sockfd, const struct sockaddr *addr,
    //                socklen_t addrlen);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX; // 반드시 AF_UNIX
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect() error");
        close(sockfd);
        return -1;
    }
    printf("client connect() 성공\n");

    // ssize_t send(int sockfd, const void *buf, size_t len, int flags);
    char *msg = "hello, server";
    if ((ret = send(sockfd, msg, strlen(msg), 0)) == -1) {
        perror("send() error");
        close(sockfd);
        return -1;
    }
    printf("client send() 성공, ret: %d\n", ret);

    memset(buf, 0, sizeof(buf));
    // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
    if ((ret = recv(sockfd, buf, sizeof(buf), 0)) == -1) {
        perror("recv() error");
        close(sockfd);
        return -1;
    }
    printf("client recv() 성공, ret: %d, message: %s\n", ret, buf);

    close(sockfd);
    return 0;
}

// ./socket_stream server
// ./socket_stream client
int main(int argc, char **argv) {
    int ret;

    if (argc < 2) {
        printf("Usage: %s (server or client)\n", argv[0]);
        return -1;
    }

    if (!strcmp(argv[1], "server")) {
        ret = do_server();
    } else if (!strcmp(argv[1], "client")) {
        ret = do_client();
    } else {
        printf("Usage: %s (server or client)\n", argv[0]);
        return -1;        
    }
    
    return ret;
}