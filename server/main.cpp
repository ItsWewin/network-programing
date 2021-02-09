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

int main(int argc, char **argv) {
  if (argc != 2) {
    perror("usage: tcpclient");
    exit(EXIT_FAILURE);
  }

  int sleepingTime = atoi(argv[1]);

  int listenfd;
  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_addr;
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERV_PORT);

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

  int connfd;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  if ((connfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  messageObject messages;

  for (;;) {
    int n = read(connfd, (char *) &messages, sizeof(messageObject));
    if (n < 0) {
      perror("error read");
      exit(EXIT_FAILURE);
    } else if (n == 0) {
      perror("client closed");
      exit(EXIT_FAILURE);
    }

    printf("received: %s", messages.data);
  }
}