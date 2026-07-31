#include <string.h>
#ifndef COMMON_H
#define COMMON_H

// idk why but i just feel like i need to make this without a single conditional
// statement
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

// Constant server config
#define DEFAULT_PORT 8080
#define MAX_CONNECTIONS 128
#define BUF_SIZE 10000
#define MAX_PATH_LENGTH 512
#define MAX_HEADER_SIZE 2048
#define LOG_DIR "./log/server.log"

// HTTP response templates and status codes
typedef enum {
  OK,
  NOT_FOUND,
  INTERNAL_SERVER_ERROR,
  BAD_REQUEST,
  RESPONSE_ERR
} ResponseType;
// Content
typedef enum {
  CONTENT_TYPE_HTML,
  CONTENT_TYPE_TEXT,
  CONTENT_TYPE_JSON,
  CONTENT_COUNT
} ContentType;

// Connection header
typedef enum {
  CONNECTION_CLOSE,
  CONNECTION_KEEP_ALIVE,
  CONNECTION_COUNT
} ConnectionType;

extern const char *const CONTENT[CONTENT_COUNT];
extern const char *const RESPONSE_TEMPLATE[RESPONSE_ERR];
extern const char *const CONNECTION_HEADER[CONNECTION_COUNT];

// Term text color
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m" // <- reset after every change

#endif // !COMMON_H
