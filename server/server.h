#ifndef SERVER_H
#define SERVER_H

#define MAX_BACKLOG 2000

#include "server_error.h"
#include <netdb.h>

typedef struct ListenerLoopContext ListenerLoopContext;
typedef struct SendAllLoopContext SendAllLoopContext;
typedef struct AcceptFlagManipulationContext AcceptFlagManipulationContext;
typedef int (*ListenerStateAction)(ListenerLoopContext *);
typedef void (*AcceptFlagManipAction)(AcceptFlagManipulationContext *);
struct ListenerLoopContext {
  struct addrinfo *info;
  ListenerStateAction *actions;
  ssize_t fd;
  int state;
};
struct AcceptFlagManipulationContext {
  AcceptFlagManipAction *actions;
  ssize_t flag;
  int fd;
  int returnValue;
};

int get_listener_socket(const char *port);
int accept_incoming_connection(int listener);

static nerrh_ft *gai_handler[2];

#endif // !SERVER_H
