#include "server.h"
#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

int get_listener_socket(const char *port) {
  struct addrinfo hints = {.ai_family = AF_UNSPEC,
                           .ai_socktype = SOCK_STREAM,
                           .ai_flags = AI_PASSIVE | AI_NUMERICSERV};
  struct addrinfo *servinfo, *p;

  if (getaddrinfo(NULL, port, &hints, &servinfo) != 0) {
    perror("getaddrinfo");
    exit(EXIT_FAILURE);
  }

  int listener_fd = -1;
  int yes = 1;

  for (p = servinfo; p != NULL; p = p->ai_next) {
    listener_fd =
        socket(p->ai_family, p->ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC,
               p->ai_protocol);
    if (listener_fd < 0)
      continue;

    setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener_fd, p->ai_addr, p->ai_addrlen) == 0) {
      break; // Successfully bound
    }

    close(listener_fd);
    listener_fd = -1;
  }

  freeaddrinfo(servinfo);

  if (listener_fd < 0) {
    fprintf(stderr, "Failed to bind listener socket\n");
    exit(EXIT_FAILURE);
  }

  if (listen(listener_fd, MAX_BACKLOG) < 0) {
    perror("listen");
    close(listener_fd);
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %s...\n", port);

  return listener_fd;
}

static int Io_WriteHandler(const ConnectionContext *restrict ctx) {}
static int Io_ReadHandler(const ConnectionContext *restrict ctx) {}

int epoll_handler(const int listener_fd) {
  int epfd = epoll_create1(EPOLL_CLOEXEC);
  if (epfd == -1) {
    perror("epoll_create1");
    return -1;
  }
  ConnectionContext *listener_ctx = malloc(sizeof(ConnectionContext));
  if (!listener_ctx) {
    perror("malloc: listener_ctx");
    return -1;
  }
  listener_ctx->fd = listener_fd;
  listener_ctx->is_listener = true;

  struct epoll_event ev = {.events = EPOLLIN, .data.ptr = listener_ctx};
  if (epoll_ctl(epfd, EPOLL_CTL_ADD, listener_fd, &ev) == -1) {
    perror("epoll_ctl: listener");
    return -1;
  }

  struct epoll_event events[MAX_EVENTS];

  while (1) {
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
      perror("epoll_wait");
      break;
    }
    printf("test\n");

    for (int i = 0; i < nfds; i++) {
      ConnectionContext *ctx = events[i].data.ptr;
      // accept new connection on first iteration always
      if (ctx->is_listener) {
        while (1) {
          int client_fd =
              accept4(listener_fd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
          if (client_fd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
              break;
            perror("accept4");
            break;
          }

          ConnectionContext *client_ctx = malloc(sizeof(ConnectionContext));
          if (!client_ctx) {
            close(client_fd);
            continue;
          }
          client_ctx->fd = client_fd;
          client_ctx->is_listener = false;

          struct epoll_event client_ev = {
              .events = EPOLLIN | EPOLLET | EPOLLRDHUP,
              .data.ptr = client_ctx // Store pointer to context
          };
          printf("client connected to: %d\n", client_ctx->fd);

          // add newly created epoll instance from the client
          if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_ev) < 0) {
            perror("epoll_ctl: server");
            close(client_fd);
            free(client_ctx);
          }
        }
      } else {
        // Use array of function pointers for this
        // Handle client I/O
        ConnectionContext *ctx = (ConnectionContext *)events[i].data.ptr;
        // Read/Write operations...

        printf("what flag: %d\n", events[i].events);
        printf("%d\n", ctx->fd);
        // On disconnect or error:
        // epoll_ctl(epfd, EPOLL_CTL_DEL, ctx->fd, NULL);
        // close(ctx->fd);
        // free(ctx);
      }
    }
  }

  free(listener_ctx);
  close(listener_fd);
  close(epfd);
  return 0;
}

#ifndef TESTING

int main(int argc, char *argv[]) {
  signal(SIGPIPE, SIG_IGN);

  const char *port = (argc > 1) ? argv[1] : "8080";
  int listener_fd = get_listener_socket(port);

  epoll_handler(listener_fd);

  return EXIT_SUCCESS;
}

#endif /* ifndef TESTING */
