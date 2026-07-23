#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  // hints is used to hint what sock to use and what family it is
  // *res is used for the linked list that contains the data from getaddrinfo
  // and p is the iterator, since if we use res to iterate then we will lose the
  // head or need to reverse the linked list to free it so we just use p which
  // will point to head of
  // res
  struct addrinfo hints, *res, *p;
  int status;
  // act as a buffer for presentation string
  char ipstr[INET6_ADDRSTRLEN];

  if (argc != 2) {
    fprintf(stderr, "you need to tell me what's the adddress is idiot\n");
    exit(1);
  }

  // so it doesn't read any stupid garbage data
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  // use tcp on sockstream // this is optional since sockstream is already auto
  // tcp
  hints.ai_protocol = IPPROTO_TCP;

  // the dns from arg will be taken and then build a linked list in res with the
  // template from hints and paste the IP addresses found to ai_addr
  if ((status = getaddrinfo(argv[1], "3490", &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    exit(status);
  }
  // this will return index to be used to sent system call to an open socket
  int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (sockfd == -1)
    fprintf(stderr, "socket is not working hmph!\n");

  int bindret = bind(sockfd, res->ai_addr, res->ai_addrlen);
  if (bindret == -1) {
    fprintf(stderr, "HMPHH! bind also broken\n");
    // exit(bindret);
  }
  printf("this what bind returns to me: %d\n", bindret);

  printf("IP addresses for %s:\n", argv[1]);

  // we iterate through it to check each ai_addr to based on their family
  for (p = res; p != NULL; p = p->ai_next) {
    // generic to later points to either sin_addr or sin6_addr
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
    } else {
      printf("Unknown address\n");
      continue;
    }

    // convert addr which contain p->sin_addr to human readable address and
    // copying it to ipstr
    inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
    printf(" %s: %s\n", ipver, ipstr);
  }
  // this will iterate through the linked list freeing all value
  freeaddrinfo(res);
  printf("the protocol is %d\n", sockfd);

  return EXIT_SUCCESS;
}
