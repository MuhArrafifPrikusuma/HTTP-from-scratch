#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <endian.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  struct sockaddr_storage their_addr;
  struct addrinfo hints, *res, *p;
  char ipstr[INET6_ADDRSTRLEN];
  int status, sockfd;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_DGRAM;

  if ((status = getaddrinfo(NULL, "8080", &hints, &res)) == -1) {
    fprintf(stderr, "getaddrinfo: %s", gai_strerror(status));
    return EXIT_FAILURE;
  }

  for (p = res; p != NULL; p = p->ai_next) {

    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("Failed to connect");
      continue;
    }

    if (p->ai_family == AF_INET) {
      int broadcast = 1;
      if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
                     sizeof broadcast) == -1) {
        perror("setsockopt");
        close(sockfd);
        continue;
      }
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("bind");
      close(sockfd);
      continue;
    }
    break;
  }
  if (p == NULL) {
    fprintf(stderr, "Failed to connect to any address");
    close(sockfd);
    return EXIT_FAILURE;
  }

  freeaddrinfo(res);

  while (1) {

    char bufrecv[2048];
    socklen_t addr_size = sizeof(their_addr);
    memset(bufrecv, 0, sizeof(bufrecv));
    int len = sizeof(bufrecv) - 1;
    int bytes_recv;
    if ((bytes_recv = recvfrom(sockfd, bufrecv, len, 0,
                               (struct sockaddr *)&their_addr, &addr_size)) <=
        0) {
      if (bytes_recv < 0) {
        perror("cannot receive anything somehow\n");
      }
      continue;
    }

    void *addr;
    struct sockaddr_in *v4;
    struct sockaddr_in6 *v6;
    switch (their_addr.ss_family) {

    case AF_INET:
      v4 = (struct sockaddr_in *)&their_addr;
      addr = &(v4->sin_addr);
      break;
    case AF_INET6:
      v6 = (struct sockaddr_in6 *)&their_addr;
      addr = &(v6->sin6_addr);
      break;
    default:
      fprintf(stderr, "Unknown family!");
      close(sockfd);
      return EXIT_FAILURE;
    }

    inet_ntop(their_addr.ss_family, addr, ipstr, sizeof(ipstr));

    char method[16] = {0};
    char path[256] = {0};

    sscanf(bufrecv, "%15s %255s", method, path);
    printf("Receive from %s: %s %s", ipstr, method, path);

    char body[1024];
    if (strcmp(path, "/") == 0) {
      snprintf(body, sizeof(body), "Root\n");
    } else if (strcmp(path, "/test") == 0) {
      snprintf(body, sizeof(body), "Success\n");
    } else {
      snprintf(body, sizeof(body), "404 Error: path %s not found!", path);
    }

    char response[1024];
    size_t body_len = strlen(body);
    int response_len =
        snprintf(response, sizeof(response),
                 "HTTP/1.1 %s\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: %zu\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%s",
                 (strcmp(path, "/") == 0 || strcmp(path, "/test") == 0)
                     ? "200 OK"
                     : "404 Not Found!",
                 body_len, body);

    if (sendto(sockfd, response, response_len, 0,
               (struct sockaddr *)&their_addr, addr_size) == -1) {
      perror("sendto failed");
      continue;
    }
  }

  close(sockfd);
  return EXIT_SUCCESS;
}
