#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/select.h>
#include "../ubqt.h"

int
ubqt_check_input() {
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);

  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 700000;

  int status = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

  if (status == -1 && errno != EINTR) {
    return -1;
  } else if (FD_ISSET(STDIN_FILENO, &fds)) {
    return 0;
  } else {
    return 1;
  }
}
