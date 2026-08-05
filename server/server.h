#ifndef SERVER_H
#define SERVER_H
#include "server_error.h"
#include <sys/epoll.h>
#include <sys/types.h>

#define MAX_BACKLOG 2000
#define MAX_EVENTS 128

typedef struct ListenerLoopContext ListenerLoopContext;
typedef struct SendAllLoopContext SendAllLoopContext;
typedef struct AcceptFlagManipulationContext AcceptFlagManipulationContext;
typedef struct EpollContext EpollContext;
typedef int (*ListenerStateAction)(ListenerLoopContext *);
typedef void (*AcceptFlagManipAction)(AcceptFlagManipulationContext *);
typedef void (*EpollAction)(EpollContext *);
typedef struct {
  int fd;
  char read_buffer[MAX_RECV_SIZE];
} ConnectionContext;

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
  ssize_t val;
  ssize_t returnValue;
  int8_t keep_running;
};

int get_listener_socket(const char *port);
int accept_incoming_connection(int listener);
int init_epoll_fd();
int epoll_add_events(const int fd, const int epfd, struct epoll_event *events);
static nerrh_ft *gai_handler[2];

#endif // !SERVER_H
