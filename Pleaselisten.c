#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MYPORT "8080"
#define BACKLOG 10

int main(int argc, char *argv[]) {
  struct sockaddr_storage their_addr;
  struct addrinfo hints, *res, *p;
  socklen_t addr_size;
  int status;
  char ipstr[INET6_ADDRSTRLEN];
  int sockfd = -1;
  int new_fd = -1;
  bool connect_any;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, MYPORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s", gai_strerror(status));
    return EXIT_FAILURE;
  }

  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;
    struct sockaddr_in *v4;
    struct sockaddr_in6 *v6;

    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("failed to connect");
      continue;
    }

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes))
      perror("setsockopt");

    if ((bind(sockfd, p->ai_addr, p->ai_addrlen)) == 0) {
      if (p->ai_family == AF_INET) {
        v4 = (struct sockaddr_in *)p->ai_addr;
        addr = &v4->sin_addr;
      }
      if (p->ai_family == AF_INET6) {
        v6 = (struct sockaddr_in6 *)p->ai_addr;
        addr = &v6->sin6_addr;
      }
      inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
      break;
    }

    close(sockfd);
  }
  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    return EXIT_FAILURE;
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    close(sockfd);
    return EXIT_FAILURE;
  }
  printf("Server: listening to %s\nport: %s\n", ipstr, MYPORT);

  addr_size = sizeof(their_addr);
  new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
  if (new_fd == -1) {
    perror("accept");
    close(sockfd);
    return EXIT_FAILURE;
  }
  printf("Connection accepted!\n");

  close(new_fd);
  close(sockfd);

  return EXIT_SUCCESS;
}
