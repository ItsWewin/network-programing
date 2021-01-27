#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
using namespace std;

#define SERV_PORT 63323
#define MAXLINE 4096

static int count = 10;

static void recvfrom_int(int signo) {
  cout << "received " << ::count << "datagrams" << endl;
  exit(0);
}

int main(int argc, char **argv) {
  int socket_fd;
  socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

  struct sockaddr_in server_addr;
  bzero(&server_addr, sizeof(server_addr));
  
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(SERV_PORT);

  bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));

  socklen_t client_len;
  char message[4096];
  int count = 0;

  signal(SIGINT, recvfrom_int);

  struct sockaddr_in client_addr;
  client_len = sizeof(client_addr);
  for (;;) {
    int n = recvfrom(socket_fd, message, MAXLINE, 0, (struct sockaddr *) &client_addr, &client_len);
    message[n] = 0;
    printf("recevied %d bytes: %s\n", n, message);

    char send_line[MAXLINE];
    sprintf(send_line, "Hi, %s", message);

    sendto(socket_fd, send_line, strlen(send_line), 0, (struct sockaddr *) &client_addr, client_len);

    count ++;
  }
}

