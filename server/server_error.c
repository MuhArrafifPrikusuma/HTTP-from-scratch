#include "server_error.h"
#include <stdio.h>

void if_gai_0(const int code) { return; }

void if_gai_not_0(const int code) {
  fprintf(stderr, "server: getaddrinfo: %s", gai_strerror(code));
  printf("it hit me");
  _exit(1);
}
