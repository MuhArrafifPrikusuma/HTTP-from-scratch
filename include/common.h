#include <string.h>
#ifndef COMMON_H

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
const char *const RESPONSE_TEMPLATE[RESPONSE_ERR] = {
    [OK] = "HTTP/1.1 200 OK\r\n",
    [NOT_FOUND] = "HTTP/1.1 404 Not Found\r\n",
    [INTERNAL_SERVER_ERROR] = "HTTP/1.1 500 Internal Server Error\r\n",
    [BAD_REQUEST] = "HTTP/1.1 400 Bad Request\r\n",
};

// Content
typedef enum {
  CONTENT_TYPE_HTML,
  CONTENT_TYPE_TEXT,
  CONTENT_TYPE_JSON,
  CONTENT_COUNT
} ContentType;
const char *const CONTENT[CONTENT_COUNT] = {
    [CONTENT_TYPE_HTML] = "Content-Type: text/html; charset=UTF-8\r\n",
    [CONTENT_TYPE_TEXT] = "Content-Type: text/plain; charset=UTF-8\r\n",
    [CONTENT_TYPE_JSON] = "Content-Type: application/json\r\n",
};

// Connection header
typedef enum {
  CONNECTION_CLOSE,
  CONNECTION_KEEP_ALIVE,
  CONNECTION_COUNT
} ConnectionType;
const char *const CONNECTION_HEADER[CONNECTION_COUNT] = {
    [CONNECTION_CLOSE] = "Connection: close\r\n",
    [CONNECTION_KEEP_ALIVE] = "Connection: keep-alive",
};

// Term text color
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m" // <- reset after every change

#endif // !COMMON_H
