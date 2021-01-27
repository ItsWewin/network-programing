#include<iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h>
using namespace std;

#define MESSAGE_SIZE 102400
#define SERV_PORT 63323
#define MAXLINE 4096

int main(int argc, char **argv) {
  if (argc != 2) {
    perror("usage: udpclient <IPaddress>");
    exit(EXIT_FAILURE);
  }

  int socket_fd;
  socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr_in server_addr;
  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERV_PORT);
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

  socklen_t server_len = sizeof(server_addr);

  struct sockaddr *reply_addr;
  reply_addr = (struct sockaddr *)malloc(server_len);

  char send_line[MAXLINE], recv_line[MAXLINE + 1];
  socklen_t len;
  int n;

  while (fgets(send_line, MAXLINE, stdin) != NULL) {
    int i = strlen(send_line);
    if (send_line[i - 1] == '\n') {
      send_line[i - 1] = 0;
    }

    printf("now sending %s\n", send_line);
    size_t rt = sendto(socket_fd, send_line, strlen(send_line), 0, (struct sockaddr *) &server_addr, server_len);
    if (rt < 0) {
      perror("send failed");
      exit(EXIT_FAILURE);
    }
    printf("send bytes: %zu", rt);

    len = 0;
    n = recvfrom(socket_fd, recv_line, MAXLINE, 0, reply_addr, &len);
    if (n < 0) {
      perror("recvfrom failed");
    }

    recv_line[n] = 0;
    fputs(recv_line, stdout);
    fputs("\n", stdout);
  }

  exit(EXIT_SUCCESS);
}
