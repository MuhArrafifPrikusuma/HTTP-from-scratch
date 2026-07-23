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

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "provide exactly 1 address");
    return EXIT_FAILURE;
  }

  struct addrinfo hints, *res, *p;
  int sockfd = -1;
  int status;
  bool connect_any = false;
  char ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((status = getaddrinfo(argv[1], "3490", &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return EXIT_FAILURE;
  }

  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;
    struct sockaddr_in *IPv4;
    struct sockaddr_in6 *IPv6;

    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd == -1)
      continue;
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1)
      perror("setsockopt");

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0) {
      if (p->ai_family == AF_INET) {
        IPv4 = (struct sockaddr_in *)p->ai_addr;
        addr = &(IPv4->sin_addr);
      } else if (p->ai_family == AF_INET6) {
        IPv6 = (struct sockaddr_in6 *)p->ai_addr;
        addr = &(IPv6->sin6_addr);
      }

      inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
      sleep(1);
      connect_any = true;
      printf("Successfully connect to %s\n", ipstr);
      fflush(stdout);
      close(sockfd);
      sockfd = -1;
      continue;
    }

    close(sockfd);
    sockfd = -1;
  }

  freeaddrinfo(res);
  close(sockfd);
  if (!connect_any) {
    fprintf(stderr, "Failed to connect\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
