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

void send_data(int sockfd) {

  cout << "send data" << endl;

  char *query;
  query = (char *)malloc(MESSAGE_SIZE);

  for (int i = 0; i < MESSAGE_SIZE; i++) {
    query[i] = 'a';
  }
  query[MESSAGE_SIZE] = '\0';

  const char *cp;
  cp = query;
  size_t remaining = strlen(query);
  while (remaining) {
    int n_written = send(sockfd, cp, remaining, 0);
    cout << "send into buffer" << n_written << endl;
    if (n_written <= 0) {
      perror("send failed");
      return;
    }
    remaining -= n_written;
    cp += n_written;
  }

  return;
}

int main(int argc, char **argv) {
  cout << "client main" << endl;

  int sockfd;
  struct sockaddr_in servaddr;

  if (argc != 2) {
    perror("usage: tcpclient <IPaddress>");
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(62333);
  inet_pton(AF_INET, argv[1], &servaddr.sin_port);

  cout << "ip " << argv[1] << endl;

  int connect_rt = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
  if (connect_rt < 0) {
    cout << "connect failed"
     << endl;
  } else {
    cout << "connect succeed" << endl;
  }

  send_data(sockfd);

  exit(0);
}