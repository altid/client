#define _BSD_SOURCE
#include <stdlib.h>
#include <string.h>
#include "../../src/ubqt.h"
#include "draw.h"

static char *
print_chunk(WINDOW *win, char *line, unsigned i, unsigned offset)
{

	char *start = strdup(line);
	start[i] = 0;
	wprintw(win, start);
	free(start);
	line = line + i + offset;

	return line;

}

void
ubqt_ncurses_markup(WINDOW *win, char *markup)
{

	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);

	char *md = strdup(markup);
	char *line;
	bool codeblock = false;
	bool blockquote = false;

	while((line = strsep(&md, "\n\x0d"))) {

		/* If we're in a codeblock, print */
		if (codeblock && line[0] != '`' && line[1] != '`' && line[2] != '`') {
			wprintw(win, line);
			waddch(win, '\n');
			continue;
		}

		switch (line[0]) {
			case '`':
				/* Toggle codeblock and grab next line */
				if (codeblock && line[1] == '`' && line[2] == '`') {
					wattroff(win, COLOR_PAIR(2));
					codeblock = false;
					continue;
				}

				if (!codeblock && line[1] == '`' && line[2] == '`') {
					wattron(win, COLOR_PAIR(2));
					codeblock = true;
					continue;
				}

				break;

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

			case '*':
			case '-':
				if (line[1] == ' ') {
					wprintw(win, " • ");
					line += 2;
				}

				break;;

			case ' ':
				if (line[1] == '*' || line[1] == '-') {
					wprintw(win, " • ");
					line += 2;
				}

				break;

			case '>':
				blockquote = true;
				wattron(win, COLOR_PAIR(2));
				wprintw(win, "▌");
				line += 1;
				break;

		}

		unsigned i = 0, len = strlen(line);

		tag_open.ex_bold	= false;
		tag_open.bold		= false;
		tag_open.strike		= false;
		tag_open.ex_em		= false;
		tag_open.color		= false;
		tag_open.code		= false;
		tag_open.path		= false;
		tag_open.img		= false;
		tag_open.inp		= false;
		tag_open.lnk		= false;
		tag_open.em			= false;

		/* TODO: Seems like mutating line gives us issues, possibly losing newline */
		while (i < len) {
			len = strlen(line);
			switch(line[i]) {
				case '\\':
					i += ubqt_replace_ch(&line, line[i + 1], i, 2);
					break;

				case '*':
					if (!tag_open.ex_bold && line[i + 1] == '*') {
						tag_open.ex_bold = true;
						line = print_chunk(win, line, i, 2);
						wattron(win, A_STANDOUT | A_BOLD);
						i = 0;
					}

					else if (tag_open.ex_bold && line[i + 1] == '*') {
						tag_open.ex_bold = false;
						line = print_chunk(win, line, i, 2);
						wattroff(win, A_STANDOUT | A_BOLD);
						i = 0;
					}

					else if (!tag_open.bold) {
						tag_open.bold = true;
						line = print_chunk(win, line, i, 1);
						wattron(win, A_BOLD);
						i = 0;
					}

					else if (tag_open.bold) {
						tag_open.bold = false;
						line = print_chunk(win, line, i, 1);
						wattroff(win, A_BOLD);
						i = 0;
					}

					else
						i++;

					break;

				case '_':
					if (!tag_open.ex_em && line[i + 1] == '_') {
						tag_open.ex_em = true;
						line[i] = 0;
						wprintw(win, line);
						line += (i + 2);
						wattron(win, A_ITALIC | A_STANDOUT);
						i = 0;
					}

					else if (tag_open.ex_em && line[i + 1] == '_') {
						tag_open.ex_em = false;
						line[i] = 0;
						wprintw(win, line);
						line += (i + 2);
						wattroff(win, A_ITALIC | A_STANDOUT);
						i = 0;
					}

					else if (!tag_open.em) {
						tag_open.em = true;
						line[i] = 0;
						wprintw(win, line);
						line += (i + 1);
						wattron(win, A_ITALIC);
						i = 0;
					}

					else if (tag_open.em) {
						tag_open.em = false;
						line[i] = 0;
						wprintw(win, line);
						line += (i + 1);
						wattroff(win, A_ITALIC);
						i = 0; 
					}

					else
						i++;
					break;

				case '`':
					if (!tag_open.code) {
						tag_open.code = true;
						line[i] = 0;
						wprintw(win, line);
						line += i + 1;
						wattron(win, COLOR_PAIR(2));
					}

					else if (tag_open.code) {
						tag_open.code = false;
						line[i] = 0;
						wprintw(win, line);
						line += i + 1;
						wattroff(win, COLOR_PAIR(2));
					}

					i++;
					break;

				case '~':
					//TODO: ncurses doesn't have strikethrough, shift up chars to unicode strikethrough variant
					if (!tag_open.strike && line[i + 1] == '~') {
						tag_open.strike = true;
						line[i] = 0;
						wprintw(win, line);
						line += i + 2;
						i = 0;
					}

					else if (tag_open.strike && line[i + 1] == '~') {
						tag_open.strike = false;
						line[i] = 0;
						wprintw(win, line);
						line += i + 2;
						i = 0;
					}

					else
						i++;
					break;

				default:
					i++;
			}

		}

		/* Check flags */
		if (blockquote == true) {
			blockquote = false;
			wattroff(win, COLOR_PAIR(2));
		}

		if (strlen(line) > 0)
			wprintw(win, line);

		waddch(win, '\n');
	}

	free(md);
}
