#ifndef SERVER_H
#define SERVER_H

#define MAX_BACKLOG 2000

#include "../include/common.h"
#include "server_error.h"
#include <netdb.h>

typedef struct ListenerLoopContext ListenerLoopContext;
typedef void (*ListenerStateAction)(ListenerLoopContext *);
struct ListenerLoopContext {
  int fd;
  int state;
  struct addrinfo *info;
  ListenerStateAction *actions;
};

nerrh_ft *gai_handler[2];
int keep_running = 1;

#endif // !SERVER_H
