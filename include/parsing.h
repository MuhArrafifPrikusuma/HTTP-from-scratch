#ifndef PARSING_H
#define PARSING_H

#include "common.h"

static inline int parse_status(const char *s) {
  return (s[0] - '0') * 100 + (s[1] - '0') * 10 + (s[2] - '0');
}

static inline ResponseType STATUS_CODE(const char *s) {
  int code = parse_status(s);
  switch (code) {
  case 200:
    return OK;
  case 404:
    return NOT_FOUND;
  case 500:
    return INTERNAL_SERVER_ERROR;
  case 400:
    return BAD_REQUEST;
  default:
    return RESPONSE_ERR;
  }
}

#endif // !PARSING_H
