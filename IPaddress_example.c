#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  struct addrinfo hints, *res, *p;
  int status;
  // use INET6 so it can accept both v4 and v6
  char ipstr[INET6_ADDRSTRLEN];

  if (argc != 2) {
    fprintf(stderr, "usage: showip hostname\n");
    exit(1);
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;     // unspecified might be v4 or v6
  hints.ai_socktype = SOCK_STREAM; // use stream socket

  // dual parentheses to allow variable assignment without compiler error
  // we can NULL the service since we are only getting the IP address now
  if ((status = getaddrinfo(argv[1], NULL, &hints, &res)) != 0) {
    // they should call it gay_strerror instead
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    exit(2);
  }

  printf("IP addresses for %s:\n\n", argv[1]);

  // loop through existing address until no more address left
  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;
    char *ipver;
    struct sockaddr_in *ipv4;
    struct sockaddr_in6 *ipv6;

    if (p->ai_family == AF_INET) {
      ipv4 = (struct sockaddr_in *)p->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPv4";
    } else if (p->ai_family == AF_INET6) {
      ipv6 = (struct sockaddr_in6 *)p->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPv6";
    } else
      continue;

    // translate address from addr from binary to human readable address
    // and then place it in ipstr buffer
    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    printf("%s: %s\n", ipver, ipstr);
  }
  freeaddrinfo(res);

  return EXIT_SUCCESS;
}
