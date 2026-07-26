#include <arpa/inet.h>
#include <bits/sockaddr.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  while (1) {

    inet_pton(int af, const char *restrict cp, void *restrict buf);
    htons(uint32_t hostlong);
    ntohs(uint16_t netshort);
    connect(int fd, const struct sockaddr *addr, socklen_t len);
    send(int fd, const void *buf, size_t n, int flags);
    recv(int fd, void *buf, size_t n, int flags);

    printf("waiting for request...\n");
    sleep(1);
  }
}
