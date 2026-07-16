#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  while (1) {

    connect(int fd, const struct sockaddr *addr, socklen_t len);
    send(int fd, const void *buf, size_t n, int flags);
    recv(int fd, void *buf, size_t n, int flags);

    printf("waiting for request...\n");
    sleep(1);
  }
}
