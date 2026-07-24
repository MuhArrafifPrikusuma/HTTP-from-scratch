#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MYPORT "8080"
#define BACKLOG 10

int main(int argc, char *argv[]) {
  struct sockaddr_storage their_addr;
  struct addrinfo hints, *res, *p;
  socklen_t addr_size;
  int status;
  char ipstr[INET6_ADDRSTRLEN];
  int sockfd = -1;
  int new_fd = -1;
  bool connect_any;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, MYPORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s", gai_strerror(status));
    return EXIT_FAILURE;
  }

  for (p = res; p != NULL; p = p->ai_next) {
    void *addr;
    struct sockaddr_in *v4;
    struct sockaddr_in6 *v6;

    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("failed to connect");
      continue;
    }

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes))
      perror("setsockopt");

    if ((bind(sockfd, p->ai_addr, p->ai_addrlen)) == 0) {
      if (p->ai_family == AF_INET) {
        v4 = (struct sockaddr_in *)p->ai_addr;
        addr = &v4->sin_addr;
      }
      if (p->ai_family == AF_INET6) {
        v6 = (struct sockaddr_in6 *)p->ai_addr;
        addr = &v6->sin6_addr;
      }
      inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
      break;
    }

    close(sockfd);
  }
  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    return EXIT_FAILURE;
  }

  if (listen(sockfd, BACKLOG) == -1) {
  error_while_listen:
    perror("listen");
    close(sockfd);
    return EXIT_FAILURE;
  }
  printf("Server: listening to %s\nport: %s\n", ipstr, MYPORT);

  while (1) {

    addr_size = sizeof(their_addr);
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_fd == -1) {
      perror("accept");
      close(sockfd);
      continue;
    }

    int len;

    char bufrecv[2048];
    memset(bufrecv, 0, sizeof(bufrecv));

    len = sizeof(bufrecv);
    int bytes_recv;
    if ((bytes_recv = recv(new_fd, bufrecv, len - 1, 0)) <= 0) {
      if (bytes_recv < 0)
        perror("recv");
      close(new_fd);
      continue;
    }

    char method[16] = {0};
    char path[256] = {0};

    // split bufrecv and put them to method and path
    sscanf(bufrecv, "%15s %255s", method, path);

    printf("Received request: %s %s\n", method, path);

    char body[512];
    if (strcmp(path, "/") == 0) {
      snprintf(body, sizeof(body), "Welcome to the homepage!\n");
    } else if (strcmp(path, "/hello") == 0) {
      snprintf(body, sizeof(body), "Hello Human! i am server!\n");
    } else if (strcmp(path, "/health") == 0) {
      snprintf(body, sizeof(body), "Server status: Running and Healthy!\n");
    } else {
      snprintf(body, sizeof(body), "404 Error: path '%s' not found!\n", path);
    }

    char response[1024];
    int response_len =
        snprintf(response, sizeof(response),
                 "HTTP/1.1 %s\r\n"
                 "Content-Type: text/plain\r\n"
                 "Content-Length: %zu\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%s",
                 (strcmp(path, "/") == 0 || strcmp(path, "/hello") == 0 ||
                  strcmp(path, "/health") == 0)
                     ? "200 OK"
                     : "404 Not Found",
                 strlen(body), body);

    if (send(new_fd, response, response_len, 0) == -1)
      continue;
    close(new_fd);
  }

  close(new_fd);
  close(sockfd);

  return EXIT_SUCCESS;
}
