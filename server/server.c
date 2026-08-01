#include "server.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage client_addr;
  int status, numbytes, sockfd, new_fd;

  memset(&hints, 0, sizeof hints);
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;

  char *PORT = get_port(argc, argv);
  printf("PORT: %s\n", PORT);

  EXIT_SUCCESS;
}
