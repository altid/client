/* Wherein we run the main loop */

/* TODO: Streams will have to be ignored after the initial creation */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include "ubqt.h"

#define MAX_EVENTS 16

void
ubqt_file_loop() {
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

		inotify_add_watch(in, path, IN_MODIFY|IN_CREATE|IN_DELETE);

		while(ret >= 0) {
				ret = epoll_wait(efd, events, MAX_EVENTS, -1);

				if(ret == EINTR)
						continue;
				char buf[4096]
						__attribute ((aligned(__alignof__(struct inotify_event))));
				const struct inotify_event *event;
				int len;
				char *ptr;
				for (;;) {
						len = read(in, buf, sizeof buf);
						if (len == -1 && errno != EAGAIN) {
								fprintf(stderr, "Read error\n");
								exit(EXIT_FAILURE);
						}
						
						if (len <= 0)
								break;

						for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
								event = (const struct inotify_event *) ptr;
								if (event->mask & IN_MODIFY)
										ubqt_data_update((char *)event->name);
								if (event->mask & IN_CREATE)
										ubqt_data_update((char *)event->name);
								if (event->mask & IN_DELETE)
										ubqt_data_remove((char *)event->name);
								ubqt_update_buffer();
						}
				}
						
		}
}

