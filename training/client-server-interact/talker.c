#include <arpa/inet.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BYTES 1024
char *PORT;
char *SERVER_NAME;

void make_socket(struct addrinfo *servinfo, struct addrinfo **p, int *fd) {
  for (*p = servinfo; *p != NULL; *p = (*p)->ai_next) {
    if ((*fd = socket((*p)->ai_family, (*p)->ai_socktype, (*p)->ai_protocol)) ==
        -1) {
      perror("talker: socket");
      continue;
    }
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "talker: failed to create socket");
    _exit(1);
  }
}

int main(int argc, char *argv[]) {
  struct addrinfo hints, *servinfo, *p;
  int status, sockfd, numbytes;

  if (argc != 4) {
    fprintf(stderr, "client: need atleast 4 arguments");
    return EXIT_FAILURE;
  }

  SERVER_NAME = argv[1];
  PORT = argv[2];
  char *mesg = argv[3];
  size_t mesg_size = strlen(mesg);

  memset(&hints, 0, sizeof hints);
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_family = AF_INET6;

  if ((getaddrinfo(SERVER_NAME, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "client: getaddrinfo: %s\n", gai_strerror(status));
    return EXIT_FAILURE;
  }

  make_socket(servinfo, &p, &sockfd);

  if ((numbytes = sendto(sockfd, mesg, mesg_size, 0, p->ai_addr,
                         p->ai_addrlen)) == -1) {
    perror("talker: sendto");
    return EXIT_FAILURE;
  }

  freeaddrinfo(servinfo);
  printf("talker: send %d bytes to %s\n", numbytes, SERVER_NAME);
  close(sockfd);

  return EXIT_SUCCESS;
}
