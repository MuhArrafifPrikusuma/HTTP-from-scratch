#include "server.h"
#include <fcntl.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static int get_listener_socket(const char *port);
static int sendall(int fd, const char *restrict buf, size_t *len);

int main(int argc, char *argv[]) {
  char *PORT = get_port(argc, argv);
  printf("PORT: %s\n", PORT);

  int listener_fd = get_listener_socket(PORT);

  EXIT_SUCCESS;
}

// actions helper
static int handle_Listener_bind(ListenerLoopContext *ctx) {
  return abs((bind(ctx->fd, ctx->info->ai_addr, ctx->info->ai_addrlen)));
}
static int handle_Listener_continue(ListenerLoopContext *ctx) { return 0; }
static int handle_Listener_error(ListenerLoopContext *ctx) {
  fprintf(stderr, "server: Failed to bind listening socket");
  _exit(1);
}

// take provided port from argv return listener fd when success and crash when failed
static int get_listener_socket(const char *port) {
  struct addrinfo hints, *servinfo;
  int status;
  int yes = 1;

  gai_handler[0] = if_gai_0;
  gai_handler[1] = if_gai_not_0;

  memset(&hints, 0, sizeof hints);
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(NULL, port, &hints, &servinfo);
  gai_handler[(status != 0)](status);

  ListenerStateAction Listener_action_table[] = {
      handle_Listener_continue,
      handle_Listener_bind,
  };
  ListenerStateAction New_Listener_action_table[] = {
      handle_Listener_continue,
      handle_Listener_error,
  };
  ListenerLoopContext ctx;
  ctx.actions = Listener_action_table;
  ctx.state = 0;
  ctx.fd = 0;
  ctx.info = servinfo;

  int keep_running = 1;
  while (keep_running) {
    ctx.fd = socket(ctx.info->ai_family, ctx.info->ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC,
                    ctx.info->ai_protocol);

    size_t valid_mask = ~((size_t)(ctx.fd >> (sizeof(ssize_t) * 8 - 1)));
    size_t is_valid = (valid_mask & 1);
    setsockopt(ctx.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    size_t is_error = ctx.actions[is_valid](&ctx);
    ctx.info = ctx.info->ai_next;
    ctx.state = (ctx.info != NULL);

    ctx.state = (is_error & ctx.state);
    keep_running = (is_error & ctx.state);
  }
  ctx.actions = New_Listener_action_table;
  ctx.actions[ctx.state](&ctx);

  printf("found fd: %d!\n", (int)ctx.fd);

  ctx.state = abs(listen(ctx.fd, MAX_BACKLOG));
  ctx.actions[ctx.state](&ctx);
  freeaddrinfo(servinfo);

  return ctx.fd;
}

// make sure to send all data without failing, this will change *len to the total size that was send
// successfully to later use to see how many actually get send if fails and will return -1 if err
static int sendall(int fd, const char *restrict buf, size_t *len) {
  size_t total_sent = 0;
  size_t target_len = *len;
  int status_code = 0;

  int keep_running = (*len > 0);
  while (keep_running) {
    size_t bytes_left = target_len - total_sent;

    ssize_t n = send(fd, buf + total_sent, bytes_left, 0);

    // return 0xFFFFFFFFFF on success 0x00000000 on failure
    size_t valid_mask = ~((size_t)(n >> (sizeof(ssize_t) * 8 - 1)));
    size_t is_valid = (valid_mask & 1);

    size_t bytes_left_mask = (bytes_left > 0);

    // both valid mask and bytes_left_mask must succeed or else we add 0 to total_sent
    size_t combine_mask = valid_mask & -(bytes_left_mask);
    size_t clean_n = (size_t)n & combine_mask;

    total_sent += clean_n;

    status_code |= (~valid_mask & -1);

    keep_running = is_valid & (total_sent < target_len);
  }

  *len = total_sent;

  return status_code;
}
