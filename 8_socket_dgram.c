#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>

#define SOCK_PATH    "sock_file"
#define BUF_SIZE    1024

// socket -> bind -> recvfrom -> close
int do_server() {
    int sockfd;
    struct sockaddr_un addr;
    char buf[BUF_SIZE];
    int ret;

    //int socket(int domain, int type, int protocol);
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("socket() error");
        return -1;
    }
    printf("socket() 성공\n");

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX; // 반드시 AF_UNIX
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    // 기존 소켓 파일이 있으면 삭제
    unlink(SOCK_PATH);

    // int bind(int sockfd, const struct sockaddr *addr,
    //           socklen_t addrlen);
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind() error");
        close(sockfd);
        return -1;
    }
    printf("bind() 성공\n");

    memset(buf, 0, sizeof(buf));
// ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
//                  struct sockaddr *src_addr, socklen_t *addrlen);
    ret = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, NULL);
    if (ret == -1) {
        perror("recvfrom()");
        close(sockfd);
        return -1;
    }

    printf("message from client: %s, ret: [%d] \n", buf, ret);

    close(sockfd);
    return 0;
}

// socket -> sendto -> close
int do_client() {
    int sockfd;
    struct sockaddr_un addr;
    int ret;
    
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
        perror("socket() error");
        return -1;
    }
    printf("client socket() 성공\n");

    // 주소 설정
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX; // 반드시 AF_UNIX
    strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);

    // ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
    //   const struct sockaddr *dest_addr, socklen_t addrlen);
    char *msg = "hello, server";
    ret = sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_un));
    if (ret == -1) {
        perror("send() error");
        close(sockfd);
        return -1;
    }
    printf("client sendto() 성공\n");    

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