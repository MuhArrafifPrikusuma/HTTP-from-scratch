#define _XOPEN_SOURCE 700
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BACKLOG 100
char *PORT;

// listening to -> int
int listen_to(struct addrinfo *serv) {
  struct addrinfo *p;
  int sockfd, yes;
  yes = 1;
  for (p = serv; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(serv);

  if (p == NULL) {
    fprintf(stderr, "failed to bind\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }
  return sockfd;
}

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// get a new file descriptor from accepted connection and will return -1 on err
int connect_to_client(int sockfd, struct sockaddr *Caddr, socklen_t *sa_size) {
  int new_fd = accept(sockfd, Caddr, sa_size);
  if (new_fd == -1) {
    return -1;
  }

  return new_fd;
}

void sigchld_handler(int sig) {
  (void)sig;
  int save_errno = errno;
  char mesg[] = "server: Child process terminated\n";

  while (waitpid(-1, NULL, WNOHANG) > 0)
    write(STDOUT_FILENO, mesg, sizeof(mesg) - 1);

  errno = save_errno;
}

// take signals and pass it to another helper function for that signals
void signal_Handler(struct sigaction *sa, int action) {
  switch (action) {
  case SIGCHLD:
    sa->sa_handler = sigchld_handler;
    break;
  }
  sigemptyset(&sa->sa_mask);
  sa->sa_flags = SA_RESTART;

  if (sigaction(action, sa, NULL) == -1) {
    perror("server: sigaction");
    _exit(1);
  }
}

int main(int argc, char *argv[]) {
  struct addrinfo hints, *serv;
  struct sockaddr_storage recvr_addr;
  struct sigaction sa;
  socklen_t sin_size;
  char ipaddrstr[INET6_ADDRSTRLEN];
  int status, sockfd, new_fd;

  if (argc != 2) {
    fprintf(stderr, "Please provide a valid port\n");
    return EXIT_FAILURE;
  }

  PORT = argv[1];
  memset(&hints, 0, sizeof hints);
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, PORT, &hints, &serv)) != 0) {
    fprintf(stderr, "server: getaddrinfo: %s\n", gai_strerror(status));
    return EXIT_FAILURE;
  }

  sockfd = listen_to(serv);
  printf("server: waiting for connection...\n");
  signal_Handler(&sa, SIGCHLD);

  while (1) {

    sin_size = sizeof recvr_addr;
    new_fd = connect_to_client(sockfd, (struct sockaddr *)&recvr_addr, &sin_size);
    if (new_fd == -1) {
      perror("server: connect_to_client");
      continue;
    }

    inet_ntop(recvr_addr.ss_family, get_in_addr((struct sockaddr *)&recvr_addr), ipaddrstr,
              sizeof ipaddrstr);

    printf("server: got connection from %s\n", ipaddrstr);
    pid_t pid = fork();
    if (pid < 0) {
      perror("server: fork");
      close(new_fd);
    } else if (pid == 0) {
      close(sockfd);
      if (send(new_fd, "Hello, client", 14, 0) == -1) {
        perror("server: send");
        close(new_fd);
        _exit(1);
      }
      close(new_fd);
      _exit(0);
    }
    close(new_fd);
  }

  return EXIT_SUCCESS;
}
