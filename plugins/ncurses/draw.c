#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ncurses.h>
//#include <panel.h>
#include "../../src/ubqt.h"

int row, col; 
int epollfd, evfd;

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
	
	cbreak();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	timeout(0);
	noecho();
	start_color();
	getmaxyx(stdscr, row, col);
	
	/* Set up epoll watching */
	epollfd = epoll_create1(0);
	evfd = eventfd(0, EFD_NONBLOCK);

	struct epoll_event ev;
	ev.data.fd = evfd;
	ev.events = EPOLLIN | EPOLLET;

	epoll_ctl(epollfd, EPOLL_CTL_ADD, evfd, &ev);

	ev.data.fd = STDIN_FILENO;
	ev.events = EPOLLIN | EPOLLPRI | EPOLLERR;
	//fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

	epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev); 

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

	eventfd_write(evfd, 1);
	return 0;

}

int
ubqt_draw_loop()
{
	struct epoll_event ev;
	char c = 0;

	/* Wait for our event, redraw */
	for (;;) {

		int fds = epoll_wait(epollfd, &ev, 1, -1);

		if (fds == -1 && errno != EINTR)
			return 1;

		ubqt_draw();
		
		if (fds == 0)
			continue;


		if (ev.data.fd == STDIN_FILENO) {
			c = getch();
			//ubqt_input_handle(c)
			if (c == 'q')
				break;
			addch(c);
		}
	}

	close(epollfd);
	close(evfd);

	return 0;

}

int
ubqt_draw_destroy()
{
	/* This should free any structures */
	echo();
	nocbreak();
	endwin();
	return 0;

}
