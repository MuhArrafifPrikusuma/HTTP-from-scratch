#include <stdalign.h>
#include <string.h>
#ifndef COMMON_H
#define COMM

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
#define MAX_RECV_SIZE 1024
#define MAX_HEADER_SEND 64
#define LOG_DIR "./log/server.log"

// HTTP response templates and status codes
typedef enum {
  OK,
  NOT_FOUND,
  INTERNAL_SERVER_ERROR,
  BAD_REQUEST,
  RESPONSE_COUNT,
  RESPONSE_UNKNOWN = -1
} ResponseType;

typedef struct {
  char text[MAX_HEADER_SEND];
} HeaderTemplate_t;
// Content
typedef enum {
  CONTENT_TYPE_HTML,
  CONTENT_TYPE_TEXT,
  CONTENT_TYPE_JSON,
  CONTENT_COUNT,
  CONTENT_UNKNOWN = -1
} ContentType;

typedef struct {
  char text[MAX_HEADER_SEND];
} ContentTemplate_t;

// Connection header
typedef enum {
  CONNECTION_CLOSE,
  CONNECTION_KEEP_ALIVE,
  CONNECTION_COUNT,
  CONNECTION_UNKNOWN = -1
} ConnectionType;

typedef struct {
  char text[MAX_HEADER_SEND];
} ConnectionTemplate_t;

extern const ContentTemplate_t CONTENT[CONTENT_COUNT];
extern const HeaderTemplate_t RESPONSE_HEADER_TYPE[RESPONSE_COUNT];
extern const ConnectionTemplate_t CONNECTION_TEMPLATE[CONNECTION_COUNT];

// Term text color
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m" // <- reset after every change

#endif // !COMMON_H
