/*
  chrony_sockcat - chrony socket communication debugger

  Copyright 2016, Peter Volgyesi, Vanderbilt University
*/
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SOCK_MAGIC 0x534f434b

struct sock_sample {
  struct timeval tv;
  double offset;
  int pulse;
  int leap;
  int _pad;
  int magic;
};

static void read_sample(int sockfd, char* tag)
{
  struct sock_sample sample;
  int s;

  s = recv(sockfd, &sample, sizeof (sample), 0);

  if (s < 0) {
    perror("could not read SOCK sample");
    return;
  }

  if (s != sizeof (sample)) {
    printf("unexpected length of SOCK sample : %d != %ld\n", s, (long)sizeof (sample));
    return;
  }

  if (sample.magic != SOCK_MAGIC) {
    printf("Unexpected magic number in SOCK sample : %x != %x\n", sample.magic, SOCK_MAGIC);
    return;
  }

  printf("[%s: %s] tv: %ld.%06ld, offset: %f, leap: %d\n",
      tag, (sample.pulse ? "PULSE" : "SAMPLE"),
      sample.tv.tv_sec, sample.tv.tv_usec, sample.offset, sample.leap);
}

int main(int argc, char* argv[])
{
    int i;
    int n_sockfds;
    int sockfds[FD_SETSIZE];
    char* paths[FD_SETSIZE];

    if (argc < 2) {
        printf("usage: %s <socket-path> [socket-path*]\n", argv[0]);
        exit(-1);
    }

    n_sockfds = argc - 1;

    for (i = 0; i < n_sockfds; i++) {
      struct sockaddr_un s;

      paths[i] = argv[i + 1];
      memset(&s, 0, sizeof(s));
      s.sun_family = AF_UNIX;
      if (snprintf(s.sun_path, sizeof (s.sun_path), "%s", paths[i]) >= sizeof (s.sun_path)) {
          printf("path %s is too long\n", paths[i]);
          exit(-1);
      }

      if ( (sockfds[i] = socket(AF_UNIX, SOCK_DGRAM, 0)) < -0) {
          perror("socket error");
          exit(-1);
      }
      unlink(paths[i]);

      if (bind(sockfds[i], (struct sockaddr *)&s, sizeof (s)) < 0) {
          perror("bind() failed");
          exit(-1);
      }

      printf("opened and receiving on: %s\n", paths[i]);
    }

    while (1) {
        fd_set active_fds;
        int max_fd = sockfds[0];

        FD_ZERO(&active_fds);
        for (i = 0; i < n_sockfds; i++) {
          FD_SET(sockfds[i], &active_fds);
          if (sockfds[i] > max_fd) {
            max_fd = sockfds[i];
          }
        }
        if (select(max_fd + 1 , &active_fds , NULL , NULL , NULL) > 0) {
            for (i = 0; i < n_sockfds; i++) {
              if (FD_ISSET(sockfds[i], &active_fds)) {
                read_sample(sockfds[i], paths[i]);
              }
          }
        }
        else {
          perror("select error");
          exit(-1);
        }
    }

    for (i = 0; i < n_sockfds; i++) {
      close(sockfds[i]);
      unlink(paths[i]);
    }

}



