#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT "6666"
#define BACKLOG 20

void sigchld_handler(int s) {
  (void)s; // this was sent by linux SIGCHLD and not actually needed since this
           // is just the required template by standard c library

  // we save errno to not interupt global scope while maintaining the last errno
  // value before anything changed it in main
  int saved_errno = errno;

  // wait for all child process and if they have finished then we close it and
  // WNOHANG is used to return immediately if there is no child process, and btw
  // this doesn't block the main loop since WNOHANG will immediately return the
  // PID of the child process that was terminated and will not wait for any to
  // be terminated(it will immediately return 0 if there is no child process
  // that were terminated)
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
  struct sigaction sa; // define how the kernel should behave on specific signal
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

  // store sigchld_handler function as a variable on sa.sa_handler which is a
  // function pointer
  sa.sa_handler = sigchld_handler;
  // empty the mask so no extra signal are blocked while executing
  // sigchld_handler, BUT ONLY SIGCHLD will be blocked
  sigemptyset(&sa.sa_mask);
  // restart any syscall that get interupted by this signal action
  sa.sa_flags = SA_RESTART;
  // this tell the kernel whatever SIGCHLD happend you need to run
  // sigchld_handler with
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

    // create a whole new copy of this process with fork
    // fork will return 0 to child if succeded and -1 to parents if fails, what
    // if it succeded? well it returns the pid to parent and 0 to child, right
    // here we seed pid < 0 which will never execute on child and will only
    // execute on parent if fork failed, and also pid == 0 will never true for
    // parent since it will never receive the pid of 0 and it will always be
    // true for child since it returns 0 if it successfully make a child process
    pid_t pid = fork();
    if (pid < 0) {
      perror("fork");
    } else if (pid == 0) {
      close(sockfd);
      if (send(new_fd, "Hello, World\n", 13, 0) == -1) {
        perror("send");
        close(new_fd);
        _exit(1);
      }
      close(new_fd);
      _exit(0); // <- always use exit for child process
    }
    close(new_fd);
  }

  return EXIT_SUCCESS;
}
