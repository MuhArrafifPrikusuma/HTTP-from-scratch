#ifndef SERVER_H
#define SERVER_H
#include "server_error.h"
#include <sys/types.h>

#define MAX_BACKLOG 2000
#define MAX_EVENTS 1024

typedef struct ListenerLoopContext ListenerLoopContext;
typedef struct SendAllLoopContext SendAllLoopContext;
typedef struct AcceptFlagManipulationContext AcceptFlagManipulationContext;
typedef struct EpollContext EpollContext;
typedef int (*ListenerStateAction)(ListenerLoopContext *);
typedef void (*AcceptFlagManipAction)(AcceptFlagManipulationContext *);
typedef void (*EpollAction)(EpollContext *);
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
struct EpollContext {
  EpollAction *actions;
  ssize_t efd;
  ssize_t returnValue;
  int8_t keep_running;
};

int get_listener_socket(const char *port);
int accept_incoming_connection(int listener);

static nerrh_ft *gai_handler[2];

#endif // !SERVER_H
