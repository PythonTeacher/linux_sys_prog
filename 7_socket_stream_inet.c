// socket_stream_inet.c
// Domain : IPv4 Internet Domain Socket
// Type : Stream
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP      "10.0.2.15"
#define PORT    8080
#define MAX_SIZE    1024

// socket -> bind -> listen -> accept -> send/recv -> close
int do_server() {
    int sockfd;
    struct sockaddr_in addr;
    int peerfd;
    char buf[MAX_SIZE];
    int ret;
    int optval = 1;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        return -1;
    }
    printf("socket() 성공\n");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;  // 모든 네트워크 인터페이스에서 들어오는 클라이언트 요청을 수신 (0.0.0.0), 이미 네트워크 바이트 순서로 되어 있어 변환할 필요가 없음
    addr.sin_port = htons(PORT);		// host to network short (호스트 바이트 순서에서 네트워크 바이트 순서로 값을 변환), 16비트 크기의 정수로 전송 시 네트워크 바이트 순서로 변환해야 함

    // 소켓을 닫은 후 바로 재사용할 수 없도록 TIME_WAIT 상태를 가짐
    // 소켓 옵션 설정
    // optval : 1(기능 활성화), 0(비활성화)
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt()");
        close(sockfd);
        return -1;        
    }  

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind()");
        close(sockfd);
        return -1;
    }
    printf("bind() 성공\n");

    if (listen(sockfd, 3) == -1) {
        perror("listen()");
        close(sockfd);
        return -1;
    }
    printf("listen()......\n");

    if ((peerfd = accept(sockfd, NULL, NULL)) == -1) {
        perror("accept()");
        close(sockfd);
        return -1;
    }
    printf("accept() 성공\n");

    memset(buf, 0, sizeof(buf));
    if ((ret = recv(peerfd, buf, sizeof(buf), 0)) == -1) {
        perror("recv()");
        close(sockfd);
        return -1;
    }
    printf("message from client: %s, ret: %d\n", buf, ret);

    char *msg = "hi, client";
    if ((ret = send(peerfd, msg, strlen(msg), 0)) == -1) {
        perror("send()");
        close(sockfd);
        return -1;
    }

    close(peerfd);
    close(sockfd);

    return 0;
}

// socket -> connect -> send/recv -> close
int do_client() {
    int sockfd;
    struct sockaddr_in addr;
    char buf[MAX_SIZE];
    int ret;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        return -1;
    }
    printf("socket() 성공\n");

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect()");
        close(sockfd);
        return -1;
    }
    printf("connect() 성공\n");

    char *msg = "hello, server";
    if ((ret = send(sockfd, msg, strlen(msg), 0)) == -1) {
        perror("send()");
        close(sockfd);
        return -1;
    }

    memset(buf, 0, sizeof(buf));
    if ((ret = recv(sockfd, buf, sizeof(buf), 0)) == -1) {
        perror("recv()");
        close(sockfd);
        return -1;
    }
    printf("message from server: %s, ret: %d\n", buf, ret);

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