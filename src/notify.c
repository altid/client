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

void
ubqt_file_loop(char *path)
{

		int in, efd, ret = 1;

		struct epoll_event ev, events[MAX_EVENTS];
		ev.events = EPOLLIN;

		if ((efd = epoll_create(1)) == -1)
				fprintf(stderr, "%s\n", strerror(errno));

		else if ((in = inotify_init1(IN_NONBLOCK|IN_CLOEXEC)) == -1)
				fprintf(stderr, "%s\n", strerror(errno));

		else if (!(ev.data.fd = in));

		else if ((epoll_ctl(efd, EPOLL_CTL_ADD, in, &ev)) == -1)
				fprintf(stderr, "%s\n", strerror(errno));

		else if ((inotify_add_watch(in, path, IN_MODIFY|IN_CREATE|IN_DELETE)) == -1)
				fprintf(stderr, "%s\n", strerror(errno));

		else {
				while(ret != -1) {
						if ((epoll_wait(efd, events, MAX_EVENTS, -1)) == -1) {
								fprintf(stderr, "%s\n", strerror(errno));
								continue;
						}

						char buf[4096] __attribute ((aligned(__alignof__(struct inotify_event))));
						const struct inotify_event *event;
						int len;
						char *ptr;

						for (;;) {
								if ((len = read(in, buf, sizeof buf)) == -1)
										fprintf(stderr, "%s\n", strerror(errno));
								if (len <= 0)
									break;

								for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
									event = (const struct inotify_event *) ptr;
										if (event->mask & IN_MODIFY)
												ubqt_data_update((char *)event->name, path);
										if (event->mask & IN_CREATE)
												ubqt_data_update((char *)event->name, path);
										if (event->mask & IN_DELETE)
											ubqt_data_remove((char *)event->name);
									ubqt_update_buffer();
								}
						}
				}
						
		}
}

