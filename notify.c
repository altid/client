/* Wherein we run the main loop */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include "ubqt.h"

void
ubqt_run_loop() {
	int fd, efd, ret = 1;
  struct epoll_event ev, events[10];
 
 	if (!(efd = epoll_create(sizeof(fd))))
		fprintf(stderr, "Could not initialize epoll file descriptor\n");

  ev.events = EPOLLIN;
  ev.data.fd = STDIN_FILENO;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1)
    fprintf(stderr, "Could not add stdin\n");
	 
	if (!(fd = inotify_init()))
		fprintf(stderr, "Unable to initialize inotify\n");

  ev.events = EPOLLIN|EPOLLHUP|EPOLLERR;
  ev.data.fd = fd;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev) == -1)
		fprintf(stderr, "Could not configure the epoll interface\n");

  if (!inotify_add_watch(fd, ubqt_win.path, IN_CLOSE_WRITE))
		fprintf(stderr, "Error adding path to watch descriptor\n");

	while(ret >= 0) {
		ret = epoll_wait(efd, events, 50, -1);
    
    if(ret == EINTR) 
      continue;

    int i;
 	  for (i = 0; i < ret; i++) {
    /* Read event */
      if(events[i].events & EPOLLIN) {
        if(events[i].data.fd == STDIN_FILENO) {
            ubqt_handle_keypress();
          ubqt_update_input();
        } else {
          ubqt_update_buffer();
        } 
        ubqt_refresh();
      /* Error */
      } else if (events[i].events & (EPOLLHUP | EPOLLERR)) {
        close(events[i].data.fd);
      }
    } 
	}
}
