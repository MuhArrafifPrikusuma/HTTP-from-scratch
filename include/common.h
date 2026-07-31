#ifndef COMMON_H

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
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

// HTTP status codes
#define HTTP_OK 200
#define HTTP_NOT_FOUND 404
#define HTTP_INTERLNAL_SERVER_ERROR 500
#define HTTP_BAD_REQUEST 400

// HTTP response templates
#define HTTP_200_TEMPLATE "HTTP/1.1 200 OK\r\n"
#define HTTP_404_TEMPLATE "HTTP/1.1 404 Not Found\r\n"
#define HTTP_500_TEMPLATE "HTTP/1.1 500 Internal Server Error\r\n"
#define HTTP_400_TEMPLATE "HTTP/1.1 400 Bad Request\r\n"

// Content
#define CONTENT_TYPE_HTML "Content-Type: text/html; charset=UTF-8\r\n"
#define CONTENT_TYPE_TEXT "Content-Type: text/plain; charset=UTF-8\r\n"
#define CONTENT_TYPE_JSON "Content-Type: application/json\r\n"

// Connection header
#define CONNECTION_CLOSE "Connection: close\r\n"
#define CONNECTION_KEEP_ALIVE "Connection: keep-alive\r\n"

// Term text color
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m" // <- reset after every change

#endif // !COMMON_H
