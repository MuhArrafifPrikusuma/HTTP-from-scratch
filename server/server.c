#include "server.h"
#include <stddef.h>
#include <stdio.h>
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

static EventFlags_t stripFlags(uint32_t *flags) {
  if (*flags & EPOLLRDHUP) {
    return HANG_UP;
  }
  if (*flags & EPOLLOUT) {
    *flags -= EPOLLOUT;
    return WRITE_READY;
  }
  if (*flags & EPOLLIN) {
    *flags -= EPOLLIN;
    return READ_READY;
  }
  return HANG_UP;
}

static void Io_Writer(ConnectionContext *restrict ctx) {
  ctx->write_buffer = "hello world!";
  size_t bufSize = strlen(ctx->write_buffer);

  ctx->write_bytes = write(ctx->fd, ctx->write_buffer, bufSize);
  printf("write %zu bytes with value of\n%s\n", ctx->write_bytes,
         ctx->write_buffer);
}
static void Io_Reader(ConnectionContext *restrict ctx) {
  ctx->read_bytes = read(ctx->fd, ctx->read_buffer, MAX_READ);
  printf("read %zu bytes with value of\n%s\n", ctx->read_bytes,
         ctx->read_buffer);
}

// NOTE: create a test case for this later when i made the client
int epoll_handler(const int listener_fd) {
  IoActions_f trigger_action[] = {
      [READ_READY] = Io_Reader,
      [WRITE_READY] = Io_Writer,
  };

  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof client_addr;

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

    for (int i = 0; i < nfds; i++) {
      ConnectionContext *ctx = events[i].data.ptr;
      // accept new connection on first iteration always
      if (ctx->is_listener) {
        while (1) {
          int client_fd = accept4(listener_fd, (struct sockaddr *)&client_addr,
                                  &addr_size, SOCK_NONBLOCK | SOCK_CLOEXEC);
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
              .events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP,
              .data.ptr = client_ctx // Store pointer to context
          };
          inet_ntop(client_addr.ss_family,
                    get_inet_addr((struct sockaddr *)&client_addr),
                    client_ctx->ipstr, sizeof client_ctx->ipstr);

          // NOTE: replace this with the logger function later
          printf("%s connected\nflags: %" PRIu32 "\nfd: %d\n",
                 client_ctx->ipstr, client_ev.events, client_fd);
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
        uint32_t evfs = events[i].events;

        while (evfs != 0) {
          EventFlags_t ef = stripFlags(&evfs);

          if (ef == HANG_UP) {
            printf("%s%s : fd: %d: hang up %s\n", COLOR_RED, ctx->ipstr,
                   ctx->fd, COLOR_RESET);
            close(ctx->fd);
            free(ctx);
            break;
          }

          trigger_action[ef](ctx);
        }
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
