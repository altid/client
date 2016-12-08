#define _GNU_SOURCE
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ncurses.h>
#include "draw.h"
#include "../../src/ubqt.h"

#define U_MAX_BYTES 4

int row, col;
int epfd, evfd;
WINDOW *win[6];

/*
 * Ncurses doesn't speak any particular form of markdown
 * we have to translate it when it's finally written to
 * screen - these functions simply no-op and return
 */
char *ubqt_markup_code(char *md) { return md; }
char *ubqt_markup_line(char *md, unsigned line) { (void)line; return md; }

int
ubqt_draw_init(char *title)
{

	setlocale(LC_ALL, "");

	/* Attempt to set title */
	printf("\033]0;%s\007", title);

	if (!initscr()) {
		fprintf(stderr, "Error calling initscr()\n");
		return 1;
	}

	start_color();
	cbreak();
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);
	scrollok(stdscr, TRUE);
	timeout(0);
	noecho();
	getmaxyx(stdscr, row, col);

	/* Set up epoll watching */
	epfd = epoll_create1(0);
	evfd = eventfd(0, EFD_NONBLOCK);

	struct epoll_event ev;
	ev.data.fd = evfd;
	ev.events  = EPOLLIN | EPOLLET;

	epoll_ctl(epfd, EPOLL_CTL_ADD, evfd, &ev);

	ev.data.fd = STDIN_FILENO;
	ev.events  = EPOLLIN | EPOLLET;
	fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);

	epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);

	//TODO: If :set input hidden
	ubqt_win.input = " ‣ ";

	return 0;

}

static unsigned
get_height(char *buf)
{

	if(strlen(buf) < 1)
		return 1;

	/* Simply count newlines */
	int i = 0, j = 0;
	while(buf[i] != '\0') {
		if(buf[i] == '\n' || buf[i] == '\x0d')
			j++;
		i++;
	}

	return (j) ? j : 1;
}

static unsigned
get_width(int cols, char *buf)
{

	/* Return lesser of max line size or cols / 4 */
	int i = 0, j = 0, max = 0, wid = cols / 4;
	while(buf[i] != '\0') {
		j++;

		if(j > wid)
			return wid;

		if(j > max)
			max = j;

		if(buf[i] == '\n' || buf[i] == '\x0d')
			j = 0;

		i++;
	}

	return max;

}

void
ubqt_draw()
{
	
	int bottom = 0, x = 0, y = 0, w = COLS, h = LINES;

	//clear();
	//refresh();

	//TODO: Add cursor in appropriate if here 
	if (ubqt_win.title != NULL) {
		delwin(win[0]);
		pthread_mutex_lock(&mutex);
		int lineh = get_height(ubqt_win.title);
		win[0] = newwin(lineh, w, y, x);
		ubqt_ncurses_markup(win[0], ubqt_win.title);
		wrefresh(win[0]);
		pthread_mutex_unlock(&mutex);
		y += lineh;
		h -= lineh;
	}

	if (ubqt_win.sidebar != NULL) {
		delwin(win[1]);
		pthread_mutex_lock(&mutex);
		int linew = get_width(w, ubqt_win.sidebar);
		win[1] = newwin(h, linew, y, x);
		ubqt_ncurses_markup(win[1], ubqt_win.sidebar);
		wrefresh(win[1]);
		pthread_mutex_unlock(&mutex);
		x += linew;
		w -= linew;
	}

	if (ubqt_win.tabs != NULL) {
		delwin(win[2]);
		pthread_mutex_lock(&mutex);
		int lineh = get_height(ubqt_win.tabs);
		win[2] = newwin(lineh, y, y, x);
		ubqt_ncurses_markup(win[2], ubqt_win.tabs);
		wrefresh(win[2]);
		pthread_mutex_unlock(&mutex);
		y += lineh;
		h -= lineh;
	}

	if (ubqt_win.input != NULL) {
		delwin(win[3]);
		pthread_mutex_lock(&mutex);
		win[3] = newwin(1, w, LINES - 1, x);
		wprintw(win[3], ubqt_win.input);
		wrefresh(win[3]);
		pthread_mutex_unlock(&mutex);
		bottom += 1;
		h -= 1;
	}

	if (ubqt_win.status != NULL) {
		delwin(win[4]);
		pthread_mutex_lock(&mutex);
		int lineh = get_height(ubqt_win.status);
		win[4] = newwin(lineh, w, LINES - lineh - bottom, x);
		ubqt_ncurses_markup(win[4], ubqt_win.status);
		wrefresh(win[4]);
		pthread_mutex_unlock(&mutex);
		h -= lineh;
	}

	if (ubqt_win.main != NULL) {
		delwin(win[5]);
		pthread_mutex_lock(&mutex);
		win[5] = newwin(h, w, y, x);
		ubqt_ncurses_markup(win[5], ubqt_win.main);
		wrefresh(win[5]);
		pthread_mutex_unlock(&mutex);
	}

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
	static unsigned short mask[] = {192, 224, 240};
	char *c = (char*)calloc(U_MAX_BYTES + 1, sizeof(char));

	int done = 0;

	/* If input window, give some initial data */

	/* Wait for our event, redraw */
	while (!done) {
		
		ubqt_draw();

		int fds = epoll_wait(epfd, &ev, 1, -1);

		if (fds == -1 && errno != EINTR) {
			free(c);
			return 1;
		}

		if (ev.data.fd == evfd) {
			ubqt_draw();
		}

		if (ev.data.fd == STDIN_FILENO) {
			//TODO: libicu u_getc
			unsigned i = 0, j;

			int ch = getch();
			
			if(ch == KEY_RESIZE) {
				//endwin();
				ubqt_draw();
				continue;
			}
				
			memset(c, 0, U_MAX_BYTES + 1);

			switch(ch) {

				case 127:
				case KEY_BACKSPACE:
					c[0] = '\x08';
					break;
				
				case '\n':
					c[0] = '\x0d';
					break;

				default:
					c[0] = ch;
					if ((c[0] & mask[0]) == mask[0]) i++;
					if ((c[0] & mask[1]) == mask[1]) i++;
					if ((c[0] & mask[2]) == mask[2]) i++;

					j = 0;
					while (j < i) {
						j++;
						c[j] = getc(stdin);
					}

					break;
			}

			done = ubqt_input_handle(c);

		}
	}

	close(epfd);
	close(evfd);
	free(c);
	return 0;

}

int
ubqt_draw_destroy()
{

	echo();
	nocbreak();
	endwin();
	return 0;

}
