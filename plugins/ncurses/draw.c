#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ncurses.h>
//#include <panel.h>
#include "../../src/ubqt.h"

int row, col; 
int epollfd, evntfd;

char *
ubqt_markup_code(char *md)
{

	return md;

}

char *
ubqt_markup_line(char *md, unsigned line)
{

	(void) line;
	return md;

}

int
ubqt_draw_init(char *title)
{
	if (!initscr()) {
		fprintf(stderr, "Error calling initscr()\n");
		return 1;
	}
	
	/* Attempt to set the window title */
	printf("\033];%s\007", title);
	
	keypad(stdscr, TRUE);
	timeout(0);
	noecho();
	start_color();
	getmaxyx(stdscr, row, col);
	
	/* Set up epoll watching */
	epollfd = epoll_create1(0);
	evntfd = eventfd(0, EFD_NONBLOCK);

	struct epoll_event evnt;
	evnt.data.fd = evntfd;
	evnt.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, evntfd, &evnt);

	evnt.data.fd = STDIN_FILENO;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &evnt); 

	return 0;

}

void
ubqt_draw()
{

	//stdscr
	if (ubqt_win.title != NULL) {
		//TODO: create window
		pthread_mutex_lock(&mutex);
		//TODO: draw window from data
		pthread_mutex_unlock(&mutex);
		//TODO: update any data to reflect new sizes
	}
	if (ubqt_win.sidebar != NULL) {
		//TODO: create window
		pthread_mutex_lock(&mutex);
		//TODO: draw window from data
		pthread_mutex_unlock(&mutex);
		//TODO: update any data to reflect new sizes
	}
	if (ubqt_win.tabs != NULL) {
		//TODO: create window
		pthread_mutex_lock(&mutex);
		//TODO: draw window from data
		pthread_mutex_unlock(&mutex);
		//TODO: update any data to reflect new sizes
	}
	if (ubqt_win.input != NULL) {
		//TODO: create window
		pthread_mutex_lock(&mutex);
		//TODO: draw window from data
		pthread_mutex_unlock(&mutex);
		//TODO: update any data to reflect new sizes
	}
	if (ubqt_win.status != NULL) {
		//TODO: create window
		pthread_mutex_lock(&mutex);
		//TODO: draw window from data
		pthread_mutex_unlock(&mutex);
		//TODO: update any data to reflect new sizes
	}
	if (ubqt_win.main != NULL) {
		//TODO: create window
		pthread_mutex_lock(&mutex);
		//TODO: draw window from data
		pthread_mutex_unlock(&mutex);
		//TODO: update any data to reflect new sizes
	}

	//TODO: Then update and draw to screen

}

int
ubqt_draw_new_data_callback()
{

	eventfd_write(evntfd, 1);
	return 0;

}

int
ubqt_draw_loop()
{
	static const int EVENTS = 20;
	struct epoll_event evnts[EVENTS];
	int i;
	char c = 0;

	/* Wait for our event, redraw */
	while(c != 'q') {
		int count = epoll_wait(epollfd, evnts, EVENTS, -1);

		if (count == -1 && errno != EINTR)
			return 1;

		for (i = 0; i < count; i++) {
			struct epoll_event *e  = evnts + i;

			if (e->data.fd == STDIN_FILENO) {
				c = getch();
				printf("%c\n", c);
			}

			ubqt_draw();

		}
	}

	return 0;

}

int
ubqt_draw_destroy()
{
	/* This should free any structures */
	echo();
	endwin();
	return 0;

}
