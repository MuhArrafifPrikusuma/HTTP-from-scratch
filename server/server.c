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
static void bind_s() { return; }

static void do_bind(int sockfd, struct addrinfo *info) {
  Loop_and_bind_ft *handle_bind[2];
  int is_err = abs(bind(sockfd, info->ai_addr, info->ai_addrlen));
  handle_bind[0] = bind_s;
}
static void do_notbind() { return; }

static int get_listener_socket(const char *PORT) {
  struct addrinfo hints, *servinfo, *p;
  int status, sockfd;
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
  Loop_and_bind_ft *handle_iteration[2];
  handle_iteration[0] = do_bind;
  handle_iteration[1] = do_notbind;

  // NOTE: figure this out later
  int i;
  while (keep_running) {
    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
    int is_error = (sockfd < 0);
    handle_iteration[is_error](sockfd, p);
  }
}
