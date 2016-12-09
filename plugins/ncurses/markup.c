#define _BSD_SOURCE
#include <stdlib.h>
#include <string.h>
#include "../../src/ubqt.h"
#include "draw.h"

void
ubqt_ncurses_markup(WINDOW *win, char *markup)
{

	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);

	char *md = strdup(markup);
	char *line;
	int codeblock = 0;

	while((line = strsep(&md, "\n\x0d"))) {

		if (codeblock && line [0] == '`' && line[1] == '`' && line[2] == '`') {
			wattroff(win, COLOR_PAIR(2));
			codeblock = 0;
			continue;
		}

		if (line [0] == '`' && line[1] == '`' && line[2] == '`') {
			wattron(win, COLOR_PAIR(2));
			codeblock = 1;
			continue;
		}

		if (codeblock) {
			wprintw(win, line);
			waddch(win, '\n');
			continue;
		}

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
				continue;

			case '>':
				wattron(win, COLOR_PAIR(2));
				wprintw(win, "▌");
				wprintw(win, line + 1);
				waddch(win, '\n');
				wattroff(win, COLOR_PAIR(2));
				continue;

		}

		int i, len = strlen(line);

		for(i = 0; i < len; i++) {
			switch(line[i]) {
				case '\\':
					i += ubqt_replace_ch(&line, line[i + 1], i, 2);
					break;
			}
		}

		wprintw(win, line);
		waddch(win, '\n');

	}

	free(md);

}
