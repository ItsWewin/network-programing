#include<iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/un.h> 
#include <string.h>
#include <sys/uio.h>
using namespace std;

typedef struct {
  u_int32_t type;
  char data[1024];
} messageObject;

#define MSG_PING 1
#define MSG_PONG 2
#define MSG_TYPE1 11
#define MSG_TYPE2 21

#define LISTENQ 1024
#define BUFFER_SIZE 4096
#define MAXLINE 4096
#define SERV_PORT 63330

#define MESSAGE_SIZE 102400
#define KEEP_ALIVE_TIME 10
#define KEEP_ALIVE_PROBETIMES 3
#define KEEP_ALIVE_INTERVAL 3;

int main(int argc, char **argv) {
    if (argc != 2) {
        perror("usage: batchwrite <IPaddress>");
        exit(EXIT_FAILURE);
    }

    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (struct sockaddr *) &server_addr, server_len);
    if (connect_rt < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    char buf[128];
    struct iovec iov[2];

    char *send_one = "hello,";
    iov[0].iov_base = send_one;
    iov[0].iov_len = strlen(send_one);
    iov[1].iov_base = buf;
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        iov[1].iov_len = strlen(buf);
        int n = htonl(iov[1].iov_len);
        if (writev(socket_fd, iov, 2) < 0)
            perror("writev failure");
            exit(EXIT_FAILURE);
    }
    exit(0);
}