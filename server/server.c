#include "server.h"
#include <netdb.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/socket.h>

static int get_listener_socket(const char *port);

// might need to move away all of this from main if i want to use multi thread for large file
// transfer
int main(int argc, char *argv[]) {
  char *PORT = get_port(argc, argv);
  printf("PORT: %s\n", PORT);

  int sockfd = get_listener_socket(PORT);
  EXIT_SUCCESS;
}

// jump functions

static void handle_bind(ListenerLoopContext *ctx) {
  ctx->current_state += abs(bind(ctx->fd, ctx->info->ai_addr, ctx->info->ai_addrlen)) + 1;
  (void)ctx->actions[ctx->current_state](ctx);
}
static void handle_continue(ListenerLoopContext *ctx) { ctx->current_state = 0; }

static void handle_break(ListenerLoopContext *ctx) {
  printf("found! listener fd: %d\n", ctx->fd);
  keep_running = 0;
}

static int get_listener_socket(const char *PORT) {
  struct addrinfo hints, *servinfo;
  int status;
  int yes = 1;

  // Listen... i couldn't figure out a better way ok?
  gai_handler[0] = if_gai_0;
  gai_handler[1] = if_gai_not_0;
  gai_handler[2] = if_gai_not_0;
  gai_handler[3] = if_gai_not_0;
  gai_handler[4] = if_gai_not_0;
  gai_handler[5] = if_gai_not_0;
  gai_handler[6] = if_gai_not_0;
  gai_handler[7] = if_gai_not_0;
  gai_handler[8] = if_gai_not_0;
  gai_handler[9] = if_gai_not_0;
  gai_handler[10] = if_gai_not_0;
  gai_handler[11] = if_gai_not_0;
  gai_handler[12] = if_gai_not_0;

  memset(&hints, 0, sizeof hints);
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;

  status = abs(getaddrinfo(NULL, PORT, &hints, &servinfo));
  gai_handler[status](status);

  ListenerStateAction Listener_action_table[] = {
      handle_bind,
      handle_continue,
      handle_break,
  };

  ListenerLoopContext ctx;
  ctx.actions = Listener_action_table;
  ctx.current_state = 0;

  ctx.info = servinfo;
  while (keep_running) {
    ctx.fd = socket(ctx.info->ai_family, ctx.info->ai_socktype, ctx.info->ai_protocol);
    ctx.current_state = (ctx.fd < 0);
    setsockopt(ctx.fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

    (void)ctx.actions[ctx.current_state](&ctx);
    ctx.info = ctx.info->ai_next;
    ctx.current_state = (ctx.info == NULL);
    (void)ctx.actions[ctx.current_state](&ctx);
  }

  return ctx.fd;
}
