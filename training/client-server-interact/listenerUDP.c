#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXBUFLEN 100
char *PORT;

void bind_sock(struct addrinfo *servinfo, int *sockfd);

void *get_addr_in(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
  struct addrinfo hints, *servinfo;
  struct sockaddr_storage ca;
  int status, numbytes, sockfd;
  socklen_t addr_len;
  char buf[MAXBUFLEN];
  char ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  if (argc != 2) {
    fprintf(stderr, "provide a valid port");
    return EXIT_FAILURE;
  }

  PORT = argv[1];

  if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "server: getaddrinfo: %s", gai_strerror(status));
    return EXIT_FAILURE;
  }

  while (1) {
    bind_sock(servinfo, &sockfd);
    printf("listener: waiting for recvfrom...\n");
    addr_len = sizeof ca;

    if ((numbytes = recvfrom(sockfd, buf, sizeof buf, 0, (struct sockaddr *)&ca, &addr_len)) ==
        -1) {
      perror("recvfrom");
      return EXIT_FAILURE;
    }

    printf("listener go packet from %s\n",
           inet_ntop(ca.ss_family, get_addr_in((struct sockaddr *)&ca), ipstr,
                     sizeof ipstr)); // <- this basically return ipstr
    printf("packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("listener: packet contain \"%s\"\n", buf);

    close(sockfd);
  }

  return EXIT_SUCCESS;
}

// will find and bind valid socket to sockfd
void bind_sock(struct addrinfo *servinfo, int *sockfd) {
  struct addrinfo *p;
  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((*sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("socket");
      continue;
    }

    if (bind(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("bind");
      close(*sockfd);
      continue;
    }
    break;
  }

  if (p == NULL) {
    fprintf(stderr, "listener: Failed to bind");
    _exit(1);
  }
}
