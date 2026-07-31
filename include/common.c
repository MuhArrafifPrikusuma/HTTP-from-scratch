#include "common.h"
#include <stdalign.h>

alignas(64) const ContentTemplate_t CONTENT[CONTENT_COUNT] = {
    [CONTENT_TYPE_HTML] = {.text = "Content-Type: text/html; charset=UTF-8\r\n"},
    [CONTENT_TYPE_TEXT] = {.text = "Content-Type: text/plain; charset=UTF-8\r\n"},
    [CONTENT_TYPE_JSON] = {.text = "Content-Type: application/json\r\n"},
};

alignas(64) const HeaderTemplate_t RESPONSE_HEADER_TYPE[RESPONSE_COUNT] = {
    [OK] = {.text = "HTTP/1.1 200 OK\r\n"},
    [NOT_FOUND] = {.text = "HTTP/1.1 404 Not Found\r\n"},
    [INTERNAL_SERVER_ERROR] = {.text = "HTTP/1.1 500 Internal Server Error\r\n"},
    [BAD_REQUEST] = {.text = "HTTP/1.1 400 Bad Request\r\n"},
};

alignas(64) const ConnectionTemplate_t CONNECTION_HEADER[CONNECTION_COUNT] = {
    [CONNECTION_CLOSE] = {.text = "Connection: close\r\n"},
    [CONNECTION_KEEP_ALIVE] = {.text = "Connection: keep-alive}\r\n"},
};
