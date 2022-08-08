#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>

int create_socket(char *addr, uint16_t port);
void set_nonblocking(int fd);

#endif
