#ifndef COMMON_H
#define COMMON_H
#define _GNU_SOURCE
// idk why but i just feel like i need to make this without a single conditional
// statement
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Constant server config
#define DEFAULT_PORT "0"
#define MAX_BUF_SIZE 2048
#define MAX_READ 4096
#define MAX_PATH_LENGTH 512
#define LOG_DIR "../log/server.log"
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

// Arrays lookups
extern const char *const CONTENT[CONTENT_COUNT];
extern const char *const RESPONSE_TYPE[RESPONSE_COUNT];
extern const char *const CONNECTION_TEMPLATE[CONNECTION_COUNT];

// inline functions

// Find the family of addr and return addr with their family format
static inline void *get_inet_addr(const struct sockaddr *restrict addr) {
  if (addr->sa_family == AF_INET)
    return &(((struct sockaddr_in *)addr)->sin_addr);
  return &(((struct sockaddr_in6 *)addr)->sin6_addr);
}

static inline uint16_t
get_inet_port(const struct sockaddr_storage *restrict addr) {
  if (addr->ss_family == AF_INET) {
    struct sockaddr_in *s = (struct sockaddr_in *)addr;
    return s->sin_port;
  }
  struct sockaddr_in6 *s = (struct sockaddr_in6 *)addr;
  return s->sin6_port;
}

static inline int getInt(const char *restrict chars, const size_t strlen,
                         const int base) {

  uint val = 0;
  for (int i = 0; i < (int)strlen; i++) {
    uint tmp_val = 0;
    tmp_val = chars[i] - '0';

    if (tmp_val < 0 || tmp_val > 9) {
      fprintf(stderr, "%d: %c is not a number\n", i, chars[i]);
      _exit(EXIT_FAILURE);
    }

    val += tmp_val * (pow(base, strlen - i - 1));
  }
  return val;
}

// Term text color
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m" // <- reset after every change

#endif // !COMMON_H
