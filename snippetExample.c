#include <arpa/inet.h>
#include <bits/sockaddr.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(void) {
  // this is inet_pton() and inet_ntop example
  // just pretend that there is an ip address here
  char IPv4buf[INET_ADDRSTRLEN];
  struct addrinfo hints;

  struct sockaddr_in sa;
  // FIX: figure this out later
  inet_pton(AF_INET, &(sa.sin_addr), IPv4buf);

  inet_ntop(AF_INET, &(sa.sin_addr), IPv4buf, INET_ADDRSTRLEN);

  printf("THE IPv4 address is: %s\n", IPv4buf);

  getaddrinfo(, const char *restrict service,
              const struct addrinfo *restrict req,
              struct addrinfo **restrict pai)
  // you can also do that with IPv6
}
