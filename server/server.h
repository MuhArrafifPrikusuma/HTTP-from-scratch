#ifndef SERVER_H
#define SERVER_H

#define MAX_BACKLOG 2000

#include "../include/common.h"
#include "server_error.h"
#include <netdb.h>

typedef struct ListenerLoopContext ListenerLoopContext;
typedef struct SendAllLoopContext SendAllLoopContext;
typedef void (*ListenerStateAction)(ListenerLoopContext *);
typedef void (*SendAllStateAction)(int *);
struct ListenerLoopContext {
  struct addrinfo *info;
  ListenerStateAction *actions;
  int fd;
  int state;
};
struct SendAllLoopContext {
  SendAllStateAction *actions;
  int state;
  int returnValue;
};

nerrh_ft *gai_handler[2];
int keep_running = 1;

#endif // !SERVER_H
