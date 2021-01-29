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

#define MESSAGE_SIZE 102400
#define SERV_PORT 63323
#define MAXLINE 4096

int main(int argc, char **argv) {
  if (argc != 2) {
    perror("usage: unixstreamclint <local_path>");
  }

  int sockfd;
  struct sockaddr_un servaddr;

  sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("create socket failed");
    exit(EXIT_FAILURE);
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sun_family = AF_LOCAL;

  strcpy(servaddr.sun_path, argv[1]);

  if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    perror("connect failed");
    exit(EXIT_FAILURE);
  }

  char send_line[MAXLINE];
  bzero(send_line, MAXLINE);
  char recv_line[MAXLINE];

  while(fgets(send_line, MAXLINE, stdin) != NULL) {
    
    int nbytes = sizeof(send_line);
    if (write(sockfd, send_line, nbytes) != nbytes) {
      perror("write error");
      exit(EXIT_FAILURE);
    }

    if (read(sockfd, recv_line, MAXLINE) == 0) {
      perror("server terminated prematurely");
       exit(EXIT_FAILURE);
    }

    fputs(recv_line, stdout);
  }

  exit(0);
}