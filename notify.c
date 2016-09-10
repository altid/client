/* Wherein we run the main loop */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include "ubqt.h"

void
ubqt_handle_event(int ret, struct epoll_event *ev) {
 	int i;
 	for (i = 0; i < ret; i++) {
		/* On any event run handler */
		//ubqt_handle_keypress(int ch)

	}
}

void
ubqt_run_loop() {
	int fd, efd, ret;
	struct epoll_event ev;

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);

	if (!(fd = inotify_init()))
		fprintf(stderr, "Unable to initialize inotify\n");

	if (!inotify_add_watch(fd, ubqt_win.path, IN_MODIFY|IN_CREATE))
		fprintf(stderr, "Error adding path to watch descriptor\n");

	inotify_add_watch(fd, STDIN_FILENO, IN_MODIFY);

	if (!(efd = epoll_create(sizeof(fd))))
		fprintf(stderr, "Could not initialize epoll file descriptor\n");

	ev.events = EPOLLIN|EPOLLOUT|EPOLLET;
	if (!epoll_ctl(efd, EPOLL_CTL_ADD, fd, &ev))
		fprintf(stderr, "Could not configure the epoll interface\n");

	while((ret = epoll_wait(efd, &ev, 100, 100000)) >= 0) {
		ubqt_handle_event(ret, &ev);
	}
}
