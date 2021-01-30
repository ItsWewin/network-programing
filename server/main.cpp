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
    perror("unage: unixdateserver <local_path>");
    exit(EXIT_FAILURE);
  }

  int socket_fd;
  socket_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
  if (socket_fd < 0) {
    perror("socket create failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_un servaddr;
  char *local_path = argv[1];
  unlink(local_path);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sun_family = AF_LOCAL;
  strcpy(servaddr.sun_path, local_path);

  if (::bind(socket_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    perror("build failed");
    exit(EXIT_FAILURE);
  }

  char buf[BUFFER_SIZE];
  struct sockaddr_un client_addr;
  socklen_t client_len = sizeof(client_addr);

  while (1) {
    bzero(buf, sizeof(buf));
    if (recvfrom(socket_fd, buf, BUFFER_SIZE, 0, (struct sockaddr *) &client_addr, &client_len) == 0) {
      printf("client quit");
      break;
    }

    printf("Recevie: %s\n", buf);

    char send_line[MAXLINE];
    bzero(send_line, MAXLINE);
    printf(send_line, "Hi: %s", buf);

    size_t nbytes = strlen(send_line);
    if (sendto(socket_fd, send_line, nbytes, 0, (struct sockaddr *) &client_addr, client_len) != nbytes) {
      perror("sendto error");
      exit(EXIT_FAILURE);
    }
  }

  close(socket_fd);
  exit(0);
}