#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <complex.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT "6666"
#define BACKLOG 20

void sigchld_handler(int s) {
  (void)s;

  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
  int new_fd, sockfd;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;
  struct sigaction sa;
  socklen_t sin_size;
  int yes = 1;
  char str[INET6_ADDRSTRLEN];
  int status;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s", gai_strerror(status));
    return EXIT_FAILURE;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      return EXIT_FAILURE;
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }
  freeaddrinfo(servinfo);

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    return EXIT_FAILURE;
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("server: listen");
    return EXIT_FAILURE;
  }

  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("server: sigaction");
    return EXIT_FAILURE;
  }

  printf("server: waiting for connections...\n");

  while (1) {
    sin_size = sizeof(their_addr);
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
              str, sizeof(str));
    printf("server: got connection from %s\n", str);

    if (!fork()) {
      close(sockfd);
      if (send(new_fd, "Hello, World\n", 13, 0) == -1) {
        perror("send");
        close(new_fd);
        exit(0);
      }
      close(new_fd);
      return EXIT_SUCCESS;
    }
  }

  return EXIT_SUCCESS;
}
