#include "include/util.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <assert.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int create_socket(char *addr, uint16_t port) {
  struct sockaddr_in remote = {0};
  remote.sin_addr.s_addr = inet_addr(addr);
  remote.sin_family = AF_INET;
  remote.sin_port = htons(port);

  int fd = socket(PF_INET, SOCK_STREAM, 0);
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)
    return -1;

  if (bind(fd, (struct sockaddr *)&remote, sizeof(remote)) < 0)
    return -1;

  return fd;
}

void set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &(int){1}, sizeof(int));
}
