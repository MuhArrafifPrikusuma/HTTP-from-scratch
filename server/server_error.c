#include "server_error.h"
#include <stdio.h>
#include <unistd.h>

void if_gai_0(const int code) { return; }

void if_gai_not_0(const int code) {
  fprintf(stderr, "server: getaddrinfo: %s\n", gai_strerror(code));
  fflush(NULL);
  _exit(1);
}
