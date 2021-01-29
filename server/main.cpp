#include <iostream>
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h> 
#include <sys/socket.h> 
#include <sys/un.h>
#include <string.h>
using namespace std;

#define LISTENQ 1024
#define BUFFER_SIZE 4096
#define MAXLINE 4096

int main(int argc, char **argv) {
  if (argc != 2) {
    perror("usage: unixstreamserver <local_path>");
    exit(0);
  }

  int listenfd, connfd;
  socklen_t clilen;
  struct sockaddr_un cliaddr, servaddr;

  listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (listenfd < 0) {
    perror("socket create failed");
    exit(EXIT_FAILURE);
  }

  char *local_path = argv[1];
  unlink(local_path);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sun_family = AF_LOCAL;
  strcpy(servaddr.sun_path, local_path);

  if (::bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(listenfd, LISTENQ) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  }

  clilen = sizeof(cliaddr);
  if ((connfd = accept(listenfd, (struct sockaddr*) &cliaddr, &clilen)) < 0) {
    if (errno == EINTR) {
      perror("accept failed");
      exit(EXIT_FAILURE);
    } else {
      perror("accept failed");
      exit(EXIT_FAILURE);
    }
  }

  char buf[BUFFER_SIZE];

  while(1) {
    bzero(buf, sizeof(buf));
    if (read(connfd, buf, BUFFER_SIZE) == 0) {
      printf("client quit");
      break;
    }

    printf("Receive: %s", buf);

    char send_line[MAXLINE];
    sprintf(send_line, "Hi, %s", buf);

    int nbytes = sizeof(send_line);

    if (write(connfd, send_line, nbytes) != nbytes) {
      perror("write error");
      exit(EXIT_FAILURE);
    }
  }

  close(listenfd);
  close(connfd);

  exit(EXIT_SUCCESS);
}