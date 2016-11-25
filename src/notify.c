/* Wherein we run the main loop */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include "ubqt.h"

#define MAX_EVENTS 16

int
ubqt_wait_event(int in, int efd, struct epoll_event events[MAX_EVENTS], char *path)
{
	/* Ignore logging error when we send signal to kill thread */
	if((epoll_wait(efd, events, MAX_EVENTS, -1)) == -1 && errno != EINTR) {
		fprintf(stderr, "Error in epoll_wait:%s\n", strerror(errno));
		return 0;
	}

	char buf[4096] __attribute ((aligned(__alignof__(struct inotify_event))));
	const struct inotify_event *event;
	int len;
	char *ptr;

	for (;;) {
		/* Ignore logging resource busy error */
		if ((len = read(in, buf, sizeof buf)) == -1 && errno != EAGAIN) {
			fprintf(stderr, "Error in I/O read: %s\n", strerror(errno));
			return 0;
		}

		if (len <= 0)
			break;

		for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
			event = (const struct inotify_event *) ptr;

			if (event->mask & IN_MODIFY)
				ubqt_data_update((char *)event->name, path);

			else if (event->mask & IN_CREATE)
				ubqt_data_update((char *)event->name, path);

			else if (event->mask & IN_DELETE)
				ubqt_data_remove((char *)event->name);

			ubqt_update_buffer();
		}
	}

	return 1;
}

void
ubqt_notify_loop(char *path)
{

	int in, efd;

	struct epoll_event ev, events[MAX_EVENTS];
	ev.events = EPOLLIN;

	if ((efd = epoll_create(1)) == -1)
		fprintf(stderr, "Error initializing epoll: %s\n", strerror(errno));

	else if ((in = inotify_init1(IN_NONBLOCK|IN_CLOEXEC)) == -1)
		fprintf(stderr, "Error initializing inotify: %s\n", strerror(errno));

	else if (!(ev.data.fd = in));

	else if ((epoll_ctl(efd, EPOLL_CTL_ADD, in, &ev)) == -1)
		fprintf(stderr, "Error configuring epoll: %s\n", strerror(errno));

	else if ((inotify_add_watch(in, path, IN_MODIFY|IN_CREATE|IN_DELETE)) == -1)
		fprintf(stderr, "Error adding inotify watch: %s\n", strerror(errno));

	else	{
		while(ubqt_wait_event(in, efd, events, path));
		fprintf(stderr, "I/O thread returning\n");
	}
}

