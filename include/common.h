#ifndef COMMON_H
#define COMMON_H
#define _GNU_SOURCE
// idk why but i just feel like i need to make this without a single conditional
// statement
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <netdb.h>
#include <signal.h>
#include <stdalign.h>
#include <stdbool.h>
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
#define MAX_BUF_SIZE 2048
#define MAX_READ 256
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

// helper for local function
char *if_argc_1(char *arg[]);
char *if_argc_2(char *arg[]);
char *if_argc_3(char *arg[]);

// Arrays lookups
extern const char *const CONTENT[CONTENT_COUNT];
extern const char *const RESPONSE_TYPE[RESPONSE_COUNT];
extern const char *const CONNECTION_TEMPLATE[CONNECTION_COUNT];

// Term text color
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m" // <- reset after every change

#endif // !COMMON_H
