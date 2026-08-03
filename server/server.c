#include "server.h"
#include <fcntl.h>
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

static int get_listener_socket(const char *port);
static int sendall(int fd, char *buf, size_t *len);

int main(int argc, char *argv[]) {
  char *PORT = get_port(argc, argv);
  printf("PORT: %s\n", PORT);

  int listener_fd = get_listener_socket(PORT);

  EXIT_SUCCESS;
}

// actions helper
static void handle_Listener_bind(ListenerLoopContext *ctx) {
  ctx->state = (bind(ctx->fd, ctx->info->ai_addr, ctx->info->ai_addrlen) == 0) + 1;
  (void)ctx->actions[ctx->state](ctx);
}
static void handle_Listener_continue(ListenerLoopContext *ctx) { ctx->state = 0; }
static void handle_Listener_break(ListenerLoopContext *ctx) {
  printf("found! listener fd: %d\n", ctx->fd);
  keep_running = 0;
}
static void handle_Listener_error(ListenerLoopContext *ctx) {
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
      handle_Listener_bind,
      handle_Listener_continue,
      handle_Listener_break,
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

  while (keep_running) {
    ctx.fd = socket(ctx.info->ai_family, ctx.info->ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC,
                    ctx.info->ai_protocol);
    ctx.state = (ctx.fd < 0);
    setsockopt(ctx.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    (void)ctx.actions[ctx.state](&ctx);
    ctx.info = ctx.info->ai_next;
    ctx.state = (ctx.info == NULL) + 1;
    (void)ctx.actions[ctx.state](&ctx);
  }
  ctx.state = ctx.state >> 1;
  ctx.actions = New_Listener_action_table;
  ctx.actions[ctx.state](&ctx);

  ctx.state = listen(ctx.fd, MAX_BACKLOG);
  ctx.actions[ctx.state](&ctx);
  freeaddrinfo(servinfo);

  return ctx.fd;
}

// action helper
static void send_success(int *ret) { return; }
static void send_failed(int *ret) { keep_running = 0; }
static void if_send_success(int *ret) { *ret = 0; }
static void if_send_failed(int *ret) { *ret = -1; }

// make sure to send all data without failing, this will change *len to the total size that was send
// successfully to later use to see how many actually get send if fails and will return -1 if err
static int sendall(int fd, char *restrict buf, size_t *len) {
  int total_send = 0;
  int bytesLeft = *len;
  int n;
  int returnValue;

  keep_running = 1;
  SendAllStateAction action_table[] = {
      send_success,
      send_failed,
  };
  SendAllStateAction new_action_table[] = {
      if_send_success,
      if_send_failed,
  };

  SendAllLoopContext ctx;
  ctx.state = 0;
  ctx.actions = action_table;

  while (keep_running) {
    n = send(fd, buf + total_send, bytesLeft, 0);
    ctx.state = (n < 0);
    ctx.actions[ctx.state](&returnValue);
    total_send += n;
    bytesLeft -= n;

    ctx.state = (total_send >= *len);
    ctx.actions[ctx.state](&returnValue);
  }

  *len = total_send;

  return returnValue;
}
