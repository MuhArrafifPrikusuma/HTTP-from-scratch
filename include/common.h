#ifndef COMMON_H
#define COMMON_H

// idk why but i just feel like i need to make this without a single conditional
// statement
#define _XOPEN_SOURCE 700
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Constant server config
#define DEFAULT_PORT "8080"
#define BUF_SIZE 10000
#define MAX_PATH_LENGTH 512
#define MAX_RECV_SIZE 2048
#define LOG_DIR "./log/server.log"
#define MAX_CONNECTION 5000

// HTTP response templates and status codes
typedef enum {
  OK,
  NOT_FOUND,
  INTERNAL_SERVER_ERROR,
  BAD_REQUEST,
  RESPONSE_COUNT,
  RESPONSE_UNKNOWN = -1
} ResponseType;

// Content
typedef enum {
  CONTENT_TYPE_HTML,
  CONTENT_TYPE_TEXT,
  CONTENT_TYPE_JSON,
  CONTENT_COUNT,
  CONTENT_UNKNOWN = -1
} ContentType;

// Connection header
typedef enum {
  CONNECTION_CLOSE,
  CONNECTION_KEEP_ALIVE,
  CONNECTION_COUNT,
  CONNECTION_UNKNOWN = -1
} ConnectionType;

// helper for local function
char *if_argc_1(char *arg[]);
char *if_argc_2(char *arg[]);
char *if_argc_3(char *arg[]);

// Functions array
typedef char *(*which_Char_ft)(char *[]);

// Arrays lookups
extern const char *const CONTENT[CONTENT_COUNT];
extern const char *const RESPONSE_TYPE[RESPONSE_COUNT];
extern const char *const CONNECTION_TEMPLATE[CONNECTION_COUNT];

// inline functions

// return socket internet address after assigning af_family to get that family
// address
static inline void *get_addr_in(const struct sockaddr *addr) {

  // use byte offset to jump to sin_addr memory block
  static const size_t offset_table[] = {
      [AF_INET] = offsetof(struct sockaddr_in, sin_addr),
      [AF_INET6] = offsetof(struct sockaddr_in6, sin6_addr),
  };

  size_t offset = offset_table[addr->sa_family];
  // jump to the offset from sockaddr_in to sin_addr
  return (void *)((const char *)addr + offset);
}

// this doesn't have any safety net and will not check whether the input is
// valid or not
static inline char *get_port(const int argc, char *argv[]) {
  which_Char_ft argv_func[3];
  argv_func[0] = if_argc_1;
  argv_func[1] = if_argc_2;
  argv_func[2] = if_argc_3;

  char *PORT = argv_func[argc - 1](argv);
  return PORT;
}

// Term text color
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m" // <- reset after every change

#endif // !COMMON_H
