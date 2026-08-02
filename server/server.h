#ifndef SERVER_H
#define SERVER_H
#include "../include/common.h"
#include "server_error.h"
#include <netdb.h>

typedef void Loop_and_bind_ft(int, struct addrinfo *);
int keep_running = 1;

nerrh_ft *gai_handler[13];
#endif // !SERVER_H
#define SERVER_H
