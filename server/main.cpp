#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
using namespace std;

size_t readn(int fd, char *buffer, size_t size) {
  char *buffer_pointer = buffer;
  int length = size;

  while ( length > 0) {
    int result = read(fd, buffer_pointer, length);

    if (result < 0) {
      if (errno == EINTR) {
        continue;
      } else if (result == 0) {
        break;
      }
    }

    length -= result;
    buffer_pointer += result;
  }

  return (size - length);
}

// 循环读取数据，一次读取 1K
void read_data(int sockfd) {
  size_t n;
  char buf[1024];

  int time = 0;
  for (;;) {
    if ((n = readn(sockfd, buf, 1024)) == 0) {
      return;
    }

    time ++;

    cout << "1K read for: " << time << endl;

    usleep(10000);
  }
}

int main(int argc, char **argv) {

  cout << "servivce" << endl;

  int listenfd, connfd;
  socklen_t clilen;
  struct sockaddr_in cliaddr, servaddr;

  listenfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  // servaddr.sin_addr.s_addr = htonl("192.168.1.100");
  servaddr.sin_port = htons(62333);

  int bindResult = ::bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  if (bindResult < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  } else {
    cout << "bind succeed" << endl;
  }
  
  int listenResult = listen(listenfd, 1024);
  if (listenResult < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
  } else {
    cout << "listen succeed" << endl;
  }

  for (;;) {
    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
    if (connfd < 0) {
      cout << "accept failed" << endl;
    } else {
      cout << "accept succeed" << endl;
    }
    read_data(connfd);
    close(connfd);
  }
}
 
