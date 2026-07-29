#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXRECVDATA 100 // <- 100 bytes
char *PORT;
char *DEST_ADDRESS;

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
  int sockfd, numbytes;
  char buf[MAXRECVDATA];
  struct addrinfo hints, *servinfo, *p;
  int status;
  char ipstr[INET6_ADDRSTRLEN];

  if (argc != 3) {
    fprintf(stderr, "usage: client hostname\n");
    return EXIT_FAILURE;
  }
  DEST_ADDRESS = argv[1];
  PORT = argv[2];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(DEST_ADDRESS, PORT, &hints, &servinfo))) {
    fprintf(stderr, "getaddrinfo: %s", gai_strerror(status));
    return EXIT_FAILURE;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    memset(ipstr, 0, sizeof ipstr);

    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), ipstr,
              sizeof ipstr);
    printf("client: attempting connection to %s\n", ipstr);

    sleep(1);
    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("client: connect");
      close(sockfd);
      continue;
    }
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return EXIT_FAILURE;
  }

  printf("client: connected to %s\n", ipstr);

  freeaddrinfo(servinfo);

  if ((numbytes = recv(sockfd, buf, MAXRECVDATA - 1, 0)) == -1) {
    perror("recv");
    return EXIT_FAILURE;
  }

  buf[numbytes] = '\0';

  printf("client: received '%s'\n", buf);

  close(sockfd);

  return EXIT_SUCCESS;
}
