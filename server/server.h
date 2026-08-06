#ifndef SERVER_H
#define SERVER_H
#include "../include/common.h"

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
  char write_buffer[MAX_BUF_SIZE];
  char read_buffer[MAX_BUF_SIZE];
  size_t read_bytes;
  size_t write_bytes;
  char ipstr[INET6_ADDRSTRLEN];
  int fd;
  bool is_listener;
};

typedef void (*IoActions_f)(ConnectionContext *);

int get_listener_socket(const char *port);

#endif // !SERVER_H
