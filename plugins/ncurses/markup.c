#define _BSD_SOURCE
#include <stdlib.h>
#include <string.h>
#include "draw.h"

void
ubqt_ncurses_markup(WINDOW *win, char *markup)
{

	init_pair(1, COLOR_BLUE, COLOR_BLACK);

	char *md = strdup(markup);
	char *line = strtok(md, "\n\x0d");

	while(line != NULL) {
		switch (line[0]) {
			case '#':
				wattron(win, A_BOLD | COLOR_PAIR(1));
				if (line[1] != '#') {
					wattron(win, A_UNDERLINE | A_STANDOUT);
					wprintw(win, line + 1);
					wattroff(win, A_UNDERLINE | A_STANDOUT);
				}

				else if (line[2] != '#') {
					wattron(win, A_UNDERLINE);
					wprintw(win, line + 2);
					wattroff(win, A_UNDERLINE);
				}

				else if (line[3] != '#')
					wprintw(win, line + 3);

				else if (line[4] != '#')
					wprintw(win, line + 4);

				else 
					wprintw(win, line + 5);

				wattroff(win, A_BOLD | COLOR_PAIR(1));
				waddch(win, '\n');
				break;

			default:
				wprintw(win, line);
				waddch(win, '\n');
				break;
		}

		line = strtok(NULL, "\n\x0d");
	}

	free(md);

}
