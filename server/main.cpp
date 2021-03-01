#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <sys/un.h>
#include <string.h>
#include <netinet/in.h> 

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

static int count;

static void sig_int(int signo) {
    printf("\nreceived %d datagrams\n", ::count);
    exit(0);
}

char *run_cmd(char *cmd) {
    char *data = (char *)malloc(16384);
    bzero(data, sizeof(data));
    FILE *fdp;
    const int max_buffer = 256;
    char buffer[max_buffer];
    fdp = popen(cmd, "r");
    char *data_index = data;
    if (fdp) {
        while (!feof(fdp)) {
            if (fgets(buffer, max_buffer, fdp) != NULL) {
                int len = strlen(buffer);
                memcpy(data_index, buffer, len);
                data_index += len;
            }
        }
        pclose(fdp);
    }
    return data;
}

int main(int argc, char **argv) {
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt1 = ::bind(listenfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int rt2 = listen(listenfd, LISTENQ);
    if (rt2 < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    signal(SIGPIPE, SIG_IGN);

    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);


    char buf[256];
    ::count = 0;

    while (1) {
        if ((connfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
            perror("bind failed ");
            exit(EXIT_FAILURE);
        }

        while (1) {
            bzero(buf, sizeof(buf));
            int n = read(connfd, buf, sizeof(buf));
            if (n < 0) {
                perror("error read message");
                exit(EXIT_FAILURE);
            } else if (n == 0) {
                printf("client closed \n");
                close(connfd);
                break;
            }
            ::count++;
            buf[n] = 0;
            if (strncmp(buf, "ls", n) == 0) {
                char *result = run_cmd("ls");
                if (send(connfd, result, strlen(result), 0) < 0){
                    return 1;
                }
                free(result);
            } else if (strncmp(buf, "pwd", n) == 0) {
                char buf[256];
                char *result = getcwd(buf, 256);
                if (send(connfd, result, strlen(result), 0) < 0)
                    return 1;
            } else if (strncmp(buf, "cd ", 3) == 0) {
                char target[256];
                bzero(target, sizeof(target));
                memcpy(target, buf + 3, strlen(buf) - 3);
                if (chdir(target) == -1) {
                    printf("change dir failed, %s\n", target);
                }
            } else {
                char *error = "error: unknown input type";
                if (send(connfd, error, strlen(error), 0) < 0)
                    return 1;
            }
        }
    }
    exit(0);
}
