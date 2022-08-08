#include "include/util.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#undef _SENDFILE_
#ifdef _SENDFILE_
const char *RESP = "HTTP/1.1 200 OK\r\nconnection: keep-alive\r\ncontent-type: "
                   "text/html\r\ncontent-length: 1709\r\n\r\n";
#else
const char *RESP = "HTTP/1.1 200 OK\r\nconnection: keep-alive\r\ncontent-type: "
                   "text/plain\r\ncontent-length: 12\r\n\r\nHello World!";
#endif

size_t LEN = 0;

void *worker_loop(void *vargp) {
  printf("started worker\n");
  int listener = create_socket("0.0.0.0", 8080);

  if (listener < 0) {
    printf("worker exited. unable to open socket for listening.");
    return NULL;
  }

  /* struct addrinfo hints, *res; */
  /* memset(&hints, 0, sizeof(hints)); */
  /*  */
  /* hints.ai_family = AF_INET; */
  /* hints.ai_socktype = SOCK_STREAM; */
  /* hints.ai_flags = AI_PASSIVE; */

  /* int status = 0; */

  /* if ((status = getaddrinfo("0.0.0.0", NULL, &hints, &p)) < 0) */
  /*   printf("getaddrinfo %s\n", gai_strerror(status)); */

  listen(listener, 8192);

  struct epoll_event ev, events[8192];
  int nfds, epollfd;

  epollfd = epoll_create1(0);
  if (epollfd < 0) {
    printf("failed to create epoll fd\n");
    return NULL;
  }

  ev.events = EPOLLIN;
  ev.data.fd = listener;

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) == -1) {
    printf("failed to add socket to epoll\n");
    return NULL;
  }

#ifdef _SENDFILE_
  FILE *f = fopen("chat.html", "rb");
  fseek(f, 0L, SEEK_END);
  long len = ftell(f);
  fseek(f, 0L, SEEK_SET);
  fclose(f);
#endif

  printf("listen fd %d\n", listener);

  do {
    nfds = epoll_wait(epollfd, events, 8192, -1);
    if (nfds == -1) {
      printf("epoll failure\n");
    }

    for (int i = 0; i < nfds; i++) {
      int event_fd = events[i].data.fd;

      if (event_fd == listener) {
        struct sockaddr_storage client_addr;
        uint32_t size = sizeof(client_addr);

        int conn = accept(listener, (struct sockaddr *)&client_addr, &size);
        if (conn < 0) {
          printf("failed to accept conn %s\n", strerror(errno));
          exit(1);
        }

        set_nonblocking(conn);

        ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLHUP | EPOLLRDHUP;
        ev.data.fd = conn;

        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn, &ev) == -1) {
          printf("failed to register connection %s\n", strerror(errno));
        }
      } else {
        int fd = events[i].data.fd;
        char buf[8192] = {0};

        if (events[i].events & EPOLLRDHUP || events[i].events & EPOLLHUP) {
          if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
            printf("failed to delete fd %s\n", strerror(errno));
            exit(1);
          }
          close(fd);
          continue;
        }

        if (events[i].events & EPOLLIN) {
          ssize_t n = read(fd, buf, 8192);
          if (n == 0) {
            if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL) == -1) {
              printf("failed to delete fd %s\n", strerror(errno));
              continue;
            }
            close(fd);
          }
        }

        if (events[i].events & EPOLLOUT) {
          write(fd, RESP, LEN);
#ifdef _SENDFILE_
          printf("sendfile\n");
          int file = open("chat.html", O_RDONLY);
          sendfile(fd, file, NULL, (size_t)len);
          close(file);
#endif
        }
      }
    }
  } while (1);

  return NULL;
}

void handle_conn(int fd, uint32_t events) {}

int start(uint8_t num_cores, char *addr, int port) {
  pthread_t *handles = malloc(sizeof(pthread_t) * num_cores);

  for (int i = 0; i < num_cores; i++) {
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, worker_loop, NULL);
    handles[i] = thread_id;
  }

  for (int i = 0; i < num_cores; i++) {
    pthread_join(handles[i], NULL);
    printf("joined %d\n", i);
  }

  return 0;
}

int main(int argc, char **argv) {
  LEN = strlen(RESP);

  if (start(12, "127.0.0.1", 8080) != 0) {
    return -1;
  }

  return 0;
}
