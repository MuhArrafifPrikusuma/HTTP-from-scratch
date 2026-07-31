#include "common.h"

const char *const CONTENT[CONTENT_COUNT] = {
    [CONTENT_TYPE_HTML] = "Content-Type: text/html; charset=UTF-8\r\n",
    [CONTENT_TYPE_TEXT] = "Content-Type: text/plain; charset=UTF-8\r\n",
    [CONTENT_TYPE_JSON] = "Content-Type: application/json\r\n",
};

const char *const RESPONSE_TEMPLATE[RESPONSE_ERR] = {
    [OK] = "HTTP/1.1 200 OK\r\n",
    [NOT_FOUND] = "HTTP/1.1 404 Not Found\r\n",
    [INTERNAL_SERVER_ERROR] = "HTTP/1.1 500 Internal Server Error\r\n",
    [BAD_REQUEST] = "HTTP/1.1 400 Bad Request\r\n",
};

const char *const CONNECTION_HEADER[CONNECTION_COUNT] = {
    [CONNECTION_CLOSE] = "Connection: close\r\n",
    [CONNECTION_KEEP_ALIVE] = "Connection: keep-alive\r\n",
};
