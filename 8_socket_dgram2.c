#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>

#define SERVER_SOCK_PATH    "server_socket"
#define CLIENT_SOCK_PATH    "client_socket"
#define BUF_SIZE    1024

// socket -> bind -> recvfrom -> sendto -> close
int do_server() {
    int sockfd;
    struct sockaddr_un addr, client_addr;
    char buf[BUF_SIZE];
    int ret;
    socklen_t client_addr_len;

    //int socket(int domain, int type, int protocol);
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("socket() error");
        return -1;
    }
    printf("socket() 성공\n");

    // int bind(int sockfd, const struct sockaddr *addr,
    //           socklen_t addrlen);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX; // 반드시 AF_UNIX
    strncpy(addr.sun_path, SERVER_SOCK_PATH, sizeof(addr.sun_path) - 1);

    // 기존 소켓 파일이 있으면 삭제
    unlink(SERVER_SOCK_PATH);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind() error");
        close(sockfd);
        return -1;
    }
    printf("bind() 성공\n");

    memset(buf, 0, sizeof(buf));
// ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
//                  struct sockaddr *src_addr, socklen_t *addrlen);
    // 상대방 소켓의 주소로 다시 전송해야 하므로 client socket에 저장하기
    client_addr_len = sizeof(struct sockaddr_un);
    ret = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &client_addr_len);
    if (ret == -1) {
        perror("recvfrom()");
        close(sockfd);
        return -1;
    }

    printf("message from client: %s, ret: [%d] \n", buf, ret);

    char *msg = "hi, client";
    // ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
    //                   const struct sockaddr *dest_addr, socklen_t addrlen);
    // 위에서 저장한 client socket으로 전송하기
    if (sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&client_addr, client_addr_len) == -1) {
        perror("sendto()");
        close(sockfd);
        return -1;
    }
    printf("sever send() 성공\n");

    close(sockfd);
    return 0;
}

// socket -> bind -> sendto -> recvfrom -> close
int do_client() {
    int sockfd;
    struct sockaddr_un addr, server_addr;
    char buf[BUF_SIZE];
    int ret;
    
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("socket() error");
        return -1;
    }
    printf("client socket() 성공\n");

    // 기존 소켓 파일 있으면 삭제
    unlink(CLIENT_SOCK_PATH);

    // client socket 설정 및 바인딩
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX; // 반드시 AF_UNIX
    strncpy(addr.sun_path, CLIENT_SOCK_PATH, sizeof(addr.sun_path) - 1);

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind() error");
        close(sockfd);
        return -1;
    }
    printf("bind() 성공\n");

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX; // 반드시 AF_UNIX
    strncpy(server_addr.sun_path, SERVER_SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    // ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
    //   const struct sockaddr *dest_addr, socklen_t addrlen);
    char *msg = "hello, server";
    ret = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un));
    if (ret == -1) {
        perror("send() error");
        close(sockfd);
        return -1;
    }
    printf("client sendto() 성공\n");

    memset(buf, 0, sizeof(buf));
    // ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
    //                     struct sockaddr *src_addr, socklen_t *addrlen);
    ret = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
    if (ret == -1) {
        perror("recvfrom()");
        close(sockfd);
        return -1;
    }
    printf("message from server: %s, ret: [%d] \n", buf, ret);

    close(sockfd);
    return 0;
}

// ./socket_dgram server
// ./socket_dgram client
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