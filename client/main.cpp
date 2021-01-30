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
    perror("usage: unixdateclient <local_path>");
    exit(EXIT_FAILURE);
  }

  int sockfd;
  struct sockaddr_un client_addr, server_addr;

  sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("create socket failed");
    exit(EXIT_FAILURE);
  }

  bzero(&client_addr, sizeof(client_addr));
  client_addr.sun_family = AF_LOCAL;
  strcpy(client_addr.sun_path, tmpnam(NULL));

  if (::bind(sockfd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
    perror("bind failed");
  }

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sun_family = AF_LOCAL;
  strcpy(server_addr.sun_path, argv[1]);

  char send_line[MAXLINE];
  bzero(send_line, MAXLINE);
  char recv_line[MAXLINE];

  while(fgets(send_line, MAXLINE, stdin) != NULL) {
    int i = strlen(send_line);
    if (send_line[i - 1] == '\n') {
      send_line[i - 1] = 0;
    }

    size_t nbytes = strlen(send_line);
    printf("now sending %s \n", send_line);

    if (sendto(sockfd, send_line, nbytes, 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) != nbytes) {
      perror("sendto error");
    }

    int n = recvfrom(sockfd, recv_line, MAXLINE, 0, NULL, NULL);
    recv_line[n] = 0;

    fputs(recv_line, stdout);
    fputs("\n", stdout);
  }

  exit(0);
}