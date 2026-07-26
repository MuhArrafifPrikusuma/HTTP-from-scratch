#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
// bind is used to connect to a port on my local machine you can use connect if
// you want to connect to remote server
int main(int argc, char *argv[]) {
  int status;
  int sockfd = -1;
  char *port = "8080";
  struct addrinfo hints, *res, *p;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // automatically fils ip

  if ((status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return EXIT_FAILURE;
  }

  for (p = res; p != NULL; p = p->ai_next) {
    // socket syscall will open a new socket and return the file descriptor to
    // access that socket
    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1)
      continue;
    // bind the socket to the port in getaddrinfo on all local network
    // interfaces (0,0,0,0) so it can listen for connections.
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
      break;

    // close the socket
    close(sockfd);
    sockfd = -1;
  }

  // if res doesn't contain any valid address we terminate
  if (p == NULL) {
    fprintf(stderr, "Failed to bind to port: %s\n", port);
    freeaddrinfo(res);
    return EXIT_FAILURE;
  }
  printf("Successfully bind socket FD %d to port %s\n", sockfd, port);

  freeaddrinfo(res);
  close(sockfd);

  return EXIT_SUCCESS;
}
