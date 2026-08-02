#include "server.h"

// might need to move away all of this from main if i want to use multi thread for large file
// transfer
int main(int argc, char *argv[]) {
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage client_addr;
  int status, numbytes, sockfd, new_fd;

  // Listen... i couldn't figure out a better way ok?
  gai_handler[0] = if_gai_0;
  gai_handler[1] = if_gai_not_0;
  gai_handler[2] = if_gai_not_0;
  gai_handler[3] = if_gai_not_0;
  gai_handler[4] = if_gai_not_0;
  gai_handler[5] = if_gai_not_0;
  gai_handler[6] = if_gai_not_0;
  gai_handler[7] = if_gai_not_0;
  gai_handler[8] = if_gai_not_0;
  gai_handler[9] = if_gai_not_0;
  gai_handler[10] = if_gai_not_0;
  gai_handler[11] = if_gai_not_0;
  gai_handler[12] = if_gai_not_0;

  memset(&hints, 0, sizeof hints);
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_PASSIVE;

  char *PORT = get_port(argc, argv);
  printf("PORT: %s\n", PORT);

  // use bitshift xor zero and pipe the output into a function array which will fail if is not zero
  status = abs(getaddrinfo(NULL, PORT, &hints, &servinfo));
  (void)gai_handler[status](status);

  EXIT_SUCCESS;
}
