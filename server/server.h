#ifndef SERVER_H
#define SERVER_H
#include "../include/common.h"
#include "../include/library/common.h"

#define MAX_BACKLOG 2000
#define MAX_EVENTS 128

typedef enum {
  READ_READY,
  WRITE_READY,
  HANG_UP,
} EventFlags_t;

typedef struct ConnectionContext ConnectionContext;
// write global action arrays based on what flag returned
typedef int (*ConnectionAction)(ConnectionContext *);
struct ConnectionContext {
  void *reader_context;
  char *write_buffer;
  char read_buffer[MAX_READ];
  size_t read_bytes;
  size_t write_bytes;
  char ipstr[INET6_ADDRSTRLEN];
  int fd;
  bool is_listener;
};

typedef void (*IoActions_f)(ConnectionContext *);

int get_listener_socket(const char *port);
int epoll_handler(const int listener_fd);

#endif // !SERVER_H
