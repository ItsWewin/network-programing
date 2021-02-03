#include <iostream>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <string.h>
using namespace std;

#define MESSAGE_SIZE 102400
#define SERV_PORT 63330
#define LISTENQ 1024
#define MAXLINE 4096

static int count;

static void sig_int(int signo) {
  printf("\nreceived %d dategrams\n", ::count);
  exit(0);
}

int main(int argc, char **argv) {
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
  }

  int rt2 = listen(listenfd, LISTENQ);
  if (rt2 < 0) {
    perror("listen failed");
  }

  signal(SIGINT, sig_int);
  signal(SIGPIPE, SIG_IGN);

  int connfd;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);

  if ((connfd = accept(listenfd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
    perror("accept failed");
  }

  char message[MAXLINE];
  ::count = 0;

  for (;;) {
    int n = read(connfd, message, MAXLINE);
    if (n < 0) {
      perror("error read");
    } else if (n == 0) {
      perror("client closed \n");
      exit(EXIT_SUCCESS);
    }

    message[n] = 0;
    printf("received %d bytes: %s\n", n, message);
    ::count ++;

    char send_line[MAXLINE];
    sprintf(send_line, "Hi，%s", message);

    sleep(5);

    int write_nc = send(connfd, send_line, strlen(send_line), 0);
    printf("send bytes: %d \n", write_nc);
    if (write_nc < 0) {
      perror("error write");
      exit(EXIT_FAILURE);
    }
  }
}
