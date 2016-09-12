/* Wherein we run the main loop */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include "ubqt.h"

#define MAX_EVENTS 16

void
ubqt_run_loop() {
	int in, efd, link_watch, ret = 1;
  struct epoll_event ev, events[MAX_EVENTS];
 
 	if (!(efd = epoll_create(2)))
		fprintf(stderr, "Could not initialize epoll file descriptor\n");

  ev.events = EPOLLIN;
  ev.data.fd = STDIN_FILENO;
	
  if (epoll_ctl(efd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1)
    fprintf(stderr, "Could not add stdin\n");
	 
	if (!(in = inotify_init1(IN_NONBLOCK|IN_CLOEXEC)))
		fprintf(stderr, "Unable to initialize inotify\n");

  ev.events = EPOLLIN;
  ev.data.fd = in;
	if (epoll_ctl(efd, EPOLL_CTL_ADD, in, &ev) == -1)
		fprintf(stderr, "Could not configure the epoll interface\n");

  link_watch = inotify_add_watch(in, ubqt_win.path, IN_ALL_EVENTS);

	while(ret >= 0) {
		ret = epoll_wait(efd, events, MAX_EVENTS, -1);
    
    if(ret == EINTR) 
      continue;

    int i, err;
 	  for (i = 0; i < ret; i++) {
      /* inotify event */
      if(events[i].data.fd == in) {
        struct inotify_event d;
        if((err = read(in, &d, sizeof(struct inotify_event))) < 1) {
          if (errno == EAGAIN || errno == EWOULDBLOCK) {
            break;
          } else {
            fprintf(stderr, "Failed to read from directory\n");
            exit(EXIT_FAILURE);
          }
        } else if (err != sizeof(struct inotify_event)) {
          fprintf(stderr, "%d bytes, expected %lu\n", err, sizeof(struct inotify_event));
        }
        if(d.wd == link_watch) {
          inotify_rm_watch(in, link_watch);
          link_watch = inotify_add_watch(in, ubqt_win.path, IN_ALL_EVENTS|IN_DONT_FOLLOW);
        } else {
          /* We have a good fd, act on it */
          ubqt_update_buffer();
        }

      } else {     
        /* stdin event */
        ubqt_handle_keypress();
        ubqt_update_input();
      } 
      ubqt_refresh();
    } 
	}
}

