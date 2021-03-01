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

int tcp_client(char *address, int port) {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, address, &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (struct sockaddr *) &server_addr, server_len);
    if (connect_rt < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    return socket_fd;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        perror("usage: batchwrite <IPaddress>");
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[2]);
    int socket_fd = tcp_client(argv[1], port);

    char recv_line[MAXLINE], send_line[MAXLINE];
    int n;

    fd_set readmask;
    fd_set allreads;
    FD_ZERO(&allreads);
    FD_SET(0, &allreads);
    FD_SET(socket_fd, &allreads);

    for (;;) {
        readmask = allreads;
        int rc = select(socket_fd + 1, &readmask, NULL, NULL, NULL);

        if (rc <= 0) {
            perror("select failed");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(socket_fd, &readmask)) {
            n = read(socket_fd, recv_line, MAXLINE);
            if (n < 0) {
                perror("read error");
                exit(EXIT_FAILURE);
            } else if (n == 0) {
                printf("server closed\n");
                break;
            }

            recv_line[n] = 0;
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }

        if (FD_ISSET(STDIN_FILENO, &readmask)) {
            if (fgets(send_line, MAXLINE, stdin) != NULL) {
                int i = strlen(send_line);
                if (send_line[i - 1] == '\n') {
                    send_line[i - 1] = 0;
                }

                if (strncmp(send_line, "quit", strlen(send_line)) == 0) {
                    if (shutdown(socket_fd, 1)) {
                        perror("shutdown failed");
                        exit(EXIT_FAILURE);
                    }
                }

                size_t rt = write(socket_fd, send_line, strlen(send_line));
                if (rt < 0) {
                    perror("write failed");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    exit(0);
}