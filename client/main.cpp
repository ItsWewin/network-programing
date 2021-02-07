#include<iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/un.h> 
#include <string.h>
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

static int count;

int main(int argc, char **argv) {
  if (argc != 2) {
    perror("usage: tcpclient <IPaddress>");
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

  char recv_line[MAXLINE + 1];
  int n;

  fd_set readmask;
  fd_set allreads;

  struct timeval tv;
  int heartbeats = 0;

  tv.tv_sec = KEEP_ALIVE_TIME;
  tv.tv_usec = 0;

  messageObject messageObject;
  
  FD_ZERO(&allreads);
  FD_SET(socket_fd, &allreads);
  for (;;) {
    readmask = allreads;
    int rc = select(socket_fd + 1, &readmask, NULL, NULL, &tv);
    if (rc < 0) {
      perror("select failed");
      exit(EXIT_FAILURE);
    }

    if (rc == 0) {
      if (++heartbeats > KEEP_ALIVE_PROBETIMES) {
        perror("connection dead\n");
        exit(EXIT_FAILURE);
      }

      printf("sending heartbeat #%d\n", heartbeats);
      messageObject.type = htonl(MSG_PING);
      rc = send(socket_fd, (char *) &messageObject, sizeof(messageObject), 0);
      if (rc < 0) {
        perror("send failure");
        exit(EXIT_FAILURE);
      }
      tv.tv_sec = KEEP_ALIVE_INTERVAL;
      continue;
    }
    if (FD_ISSET(socket_fd, &readmask)) {
      n = read(socket_fd, recv_line, MAXLINE);
      if (n < 0) {
        perror("read error");
        exit(EXIT_FAILURE);
      } else if (n == 0) {
        perror("server terminated \n");
        exit(EXIT_FAILURE);
      }

      printf("received heartbeat, make heartbeats to 0 \n");
      heartbeats = 0;
      tv.tv_sec = KEEP_ALIVE_TIME;
    }
  }
}