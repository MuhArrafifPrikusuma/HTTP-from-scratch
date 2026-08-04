#include "server.h"
#include <bits/types/sigset_t.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <time.h>

int get_listener_socket(const char *port);
static int sendall(int fd, const char *restrict buf, size_t *len);
int accept_incoming_connection(int listener);

#ifndef TESTING

int main(int argc, char *argv[]) {
  char *PORT = get_port(argc, argv);
  printf("PORT: %s\n", PORT);

  int listener_fd = get_listener_socket(PORT);

  while (1) {
    int test = accept_incoming_connection(listener_fd);
    printf("accepted fd: %d\n", test);
  }

  EXIT_SUCCESS;
}

#endif /* ifdef TESTING */

// actions helper
static int handle_Listener_bind(ListenerLoopContext *ctx) {
  return abs((bind(ctx->fd, ctx->info->ai_addr, ctx->info->ai_addrlen)));
}
static int handle_Listener_continue(ListenerLoopContext *ctx) { return 0; }
static int handle_Listener_error(ListenerLoopContext *ctx) {
  fprintf(stderr, "server: Failed to bind listening socket");
  _exit(1);
}

// take provided port from argv return listener fd when success and crash when
// failed
int get_listener_socket(const char *port) {
  struct addrinfo hints, *servinfo;
  int status;
  int yes = 1;

  gai_handler[0] = if_gai_0;
  gai_handler[1] = if_gai_not_0;

  memset(&hints, 0, sizeof hints);
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

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
    ctx.fd = socket(ctx.info->ai_family,
                    ctx.info->ai_socktype | SOCK_NONBLOCK | SOCK_CLOEXEC,
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
  freeaddrinfo(servinfo);
  ctx.actions = New_Listener_action_table;
  ctx.actions[ctx.state](&ctx);

  printf("found fd: %d!\n", (int)ctx.fd);

  ctx.state = abs(listen(ctx.fd, MAX_BACKLOG));
  ctx.actions[ctx.state](&ctx);

  return ctx.fd;
}

// make sure to send all data without failing, this will change *len to the
// total size that was send successfully to later use to see how many actually
// get send if fails and will return -1 if err
static int sendall(int fd, const char *restrict buf, size_t *len) {
  size_t total_sent = 0;
  size_t target_len = *len;
  int status_code = 0;

  int keep_running = (target_len > 0);
  while (keep_running) {
    size_t bytes_left = target_len - total_sent;

    ssize_t n = send(fd, buf + total_sent, bytes_left, 0);

    // return 0xFFFFFFFFFF on success 0x00000000 on failure
    size_t valid_mask = ~((size_t)(n >> (sizeof(ssize_t) * 8 - 1)));
    size_t is_valid = (valid_mask & 1);
    size_t bytes_left_mask = (bytes_left > 0);

    // both valid mask and bytes_left_mask must succeed or else we add 0 to
    // total_sent
    size_t combine_mask = valid_mask & -(bytes_left_mask);
    size_t clean_n = (size_t)n & combine_mask;

    total_sent += clean_n;
    status_code |= (~valid_mask & -1);
    keep_running = is_valid & (total_sent < target_len);
  }

  *len = total_sent;

  return status_code;
}

static void accept_fcntl_action_success(AcceptFlagManipulationContext *ctx) {
  ctx->returnValue = ctx->fd;
}
static void accept_fcntl_action_failed(AcceptFlagManipulationContext *ctx) {
  ctx->returnValue = -1;
}

// non blocking function to accept incoming connection and return fd on success
// and -1 on err NOTE: if i use this function i need to also detect whether the
// connection is closed to then remove that file descriptor
int accept_incoming_connection(const int listener) {
  struct sockaddr_storage their_addr;

  AcceptFlagManipAction actions_table[] = {
      accept_fcntl_action_failed,
      accept_fcntl_action_success,
  };
  AcceptFlagManipulationContext ctx;
  ctx.actions = actions_table;
  ctx.returnValue = 0;

  socklen_t addr_size = sizeof their_addr;
  ctx.fd = accept(listener, (struct sockaddr *)&their_addr, &addr_size);

  ctx.flag = fcntl(ctx.fd, F_GETFL, 0);
  size_t valid_flag = ~((size_t)(ctx.flag >> (sizeof(ctx.flag) * 8 - 1)));
  size_t is_success = (valid_flag & 1);
  ctx.actions[is_success](&ctx);

  ctx.flag = fcntl(ctx.returnValue, F_SETFL, ctx.flag | O_NONBLOCK);

  valid_flag = ~((size_t)(ctx.flag >> (sizeof(ctx.flag) * 8 - 1)));
  is_success = (valid_flag & 1);
  ctx.actions[is_success](&ctx);

  return ctx.returnValue;
}

static void epoll_fail(EpollContext *ctx) { ctx->returnValue = -1; }
static void epoll_success(EpollContext *ctx) { ctx->returnValue = ctx->efd; }

// create new epoll file descriptor, will return file descriptor on success and
// -1 on failure
int init_epoll_fd() {
  EpollAction action_table[] = {
      epoll_fail,
      epoll_success,
  };
  EpollContext ctx;
  ctx.returnValue = 0;
  ctx.keep_running = 1;
  ctx.efd = 0;
  ctx.actions = action_table;

  ctx.efd = epoll_create1(EPOLL_CLOEXEC);
  size_t valid_flag = ~((size_t)(ctx.efd >> (sizeof(int) * 8 - 1)));
  size_t is_valid = (valid_flag & 1);
  ctx.actions[is_valid](&ctx);

  return ctx.returnValue;
}
