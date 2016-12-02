#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../src/ubqt.h"

/* Thing only applicable to whole lines parsed here: Header, blockquote, list, checkbox ☑ ☐ ☑ • ‣ ▌*/

void
ubqt_nested(char *md)
{

	/* TODO: Build list based on in_list value */

}

char *
ubqt_markup_whole_line(char *md)
{

	switch(md[0]) {

		case '#':
			if(md[1] != '#')
				asprintf(&md, "%s%s%s", "<span size=\"xx-large\" weight=\"bold\">", md + 1, "</span>");

			else if(md[2] != '#')
				asprintf(&md, "%s%s%s", "<span size=\"x-large\" weight=\"bold\">",  md + 2, "</span>");

			else if(md[3] != '#')
				asprintf(&md, "%s%s%s", "<span size=\"large\" weight=\"bold\">", md + 3, "</span>");

			else if(md[4] != '#')
				asprintf(&md, "%s%s%s", "<span size=\"medium\" weight=\"bold\">", md + 4, "</span>");

			else if(md[5] != '#')
				asprintf(&md, "%s%s%s", "<span size=\"small\" weight=\"bold\">", md + 5, "</span>");

			else
				asprintf(&md, "%s%s%s", "<span size=\"x-small\" weight=\"bold\">", md + 6, "</span>");
			break;

		case '[':
			if(md[1] == 'x' && md[2] == ']') {
				asprintf(&md, "%s%s", " ☒", md + 3);
				break;
			}

			else if(md[1] == ' ' && md[2] == ']') {
				asprintf(&md, "%s%s", " ☐", md + 3);
				break;
			}

		case '*':
		case '-':
			if (md[1] == ' ') {
				tag_open.in_list = 1;
				asprintf(&md, "%s%s", " • ", md + 2);
			}

			break;
		case ' ':
			if (md[1] == ' ' && tag_open.in_list)
				ubqt_nested(md);
			break;

		case '>':
			asprintf(&md, "%s%s", "▌ ", md + 1);
			break;

		default:
			break;
	}

	return md;

}

char *
ubqt_markup_inline(char *md)
{

	//TODO: use 'int to_next' to increment anything, so we know what these random numbers all point to.
	int i = 0;
	int len = strlen(md);
	char *tmp = NULL;

	while(i < len) {
		printf("markdown %s i %d\n", md, i);
		switch(md[i]) {
			/* If we're escaping something, cut slash, and move 2 forward */
			case '\\':
				tmp = strndup(md, len);
				ubqt_substr(md, 0, i);
				asprintf(&md, "%s%s", md, tmp + (++i));
				i++;
				break;

			case '*':
				/* open strong bold */
				if (!tag_open.strong_em && md[i + 1] == '*' && (md[i - 1] == ' ' || i == 0)) {
					tag_open.strong_em = true;
					tmp = strndup(md, len);
					ubqt_substr(md, 0, i);
					asprintf(&md, "%s%s%s", md, "<span weight=\"ultrabold\">", tmp + i + 2);
					len = strlen(md);
					i += 24;
				}

				/* close strong bold tag */
				else if (tag_open.strong_em && md[i + 1] == '*') {
					tag_open.strong_em = false;
					tmp = strndup(md, len);
					ubqt_substr(md, 0, i);
					asprintf(&md, "%s%s%s", md, "</span>", tmp + i + 2);
					len = strlen(md);
					i += 6;
				}

				else if (!tag_open.em && (md[i + 1] != ' ' && md[i - 1] != ' ')) {
					tag_open.em = true;
					tmp = strndup(md, len);
					ubqt_substr(md, 0, i);
					asprintf(&md, "%s%s%s", md, "<span weight=\"bold\">", tmp + i + 1);
					len = strlen(md);
					i += 19;
				}

				else if (tag_open.em && (md[i + 1] != ' ' && md[i - 1] != ' ')) {
					tag_open.em = false;
					tmp = strndup(md, len);
					ubqt_substr(md, 0, i);
					asprintf(&md, "%s%s%s", md, "</span>", tmp + i + 1);
					len = strlen(md);
					i += 6;
				}

				i++;
				break;

			case '[':

				/* [>12345678](Hint) sets up input block using as much line as possible */
				if (md[i + 1] == '>') {
					int j;
					for (j = 0; j < 8; j++) {
						if(md[j] == ']')
							break;
					}

					tag_open.input = true;
					char *hint = strndup(md, len);
					tmp = strndup(md, len);
					ubqt_substr(md, 0, i);
					ubqt_substr(hint, i + 2, j - 2); 
					asprintf(&md, "%s%s%s%s", md, hint, ": <span underline=\"low\">", tmp + i + j + 2);
					free(hint);
					len = strlen(md);
					i += (j + 17);

				}

				else if (md[i + 1] == '#' && md[i + 8] == ']') {
					tag_open.color = true;
					char *color = strndup(md, len);
					tmp = strndup(md, len);
					ubqt_substr(md, 0, i);
					ubqt_substr(color, i + 2, 6);
					asprintf(&md, "%s%s%s%s%s", md, "<span color=\"#", color, "\">", tmp + i + 10);
					free(color);
					len = strlen(md);
					i += 22;
				}

				else if (md[i + 1] == ' ' || md[i + 1] == 'x') {
					i += 3;
					break;
				}
				else
					tag_open.square = true;

				i++;
				break;

			case '!':
				if(md[i + 1] == '[') {
					tag_open.image = true;
					//tag_open.img[-1] = malloc(sizeof(tag_open.img[-1]));
					//tag_open.img[-1].index = i + 2;
					i++;
				}

				i++;
				break;

			case ')':
				if (tag_open.color || tag_open.input) {
					tmp = strndup(md, len);
					ubqt_substr(md, 0, i);
					asprintf(&md, "%s%s%s", md, "</span>", tmp + i + 1);
					len = strlen(md);
					i += 6;
				}

				if (tag_open.color)
					tag_open.color = false;

				else if (tag_open.input)
					tag_open.input = false;

				i++;
				break;

			case ']':

				if (tag_open.input && md[i + 1] == '(') {
					tmp = strndup(md, len);
					ubqt_substr(md, 0, i - 3);
					asprintf(&md, "%s%s%s", md, "<span underline=\"low\"> ", tmp + i);
					len = strlen(md);
					i += 24;
				}

				else if (tag_open.image && md[i + 1] == '(') {
					printf("path %s\n", md);
					//tmp = strndup(md, len);
					//ubqt_substr(tmp, tag_open.img[-1].index, i - 1 - tag_open.img[-1].index);
					/* ![name](/path/to/image) */
					// tag_open.img[-1].name = name;
					// tag_open.img[-1].index = i
					// put the path here, so we can index it, parse the text by index, and profit.

				}
					/* [name](http://somelink.com) */
				else if (tag_open.path) {
						// tag_open.img[-1].str = path;
						// tag_open.img[-1].name = name;
						// may need to malloc?
				}

				i++;
				break;

			case '\n':
			case '\0':
				i = len;
				break;

			default:
				i++;
				break;
		}
		printf("After: %s\n", md);

	}

	free(tmp);

	return md;

}

char *
ubqt_markup_line(char *md)
{

	tag_open.strong_em	= false;
	tag_open.uu_line	= false;
	tag_open.u_line		= false;
	tag_open.strike		= false;
	tag_open.code		= false;
	tag_open.em			= false;

	if ((md[0] == '`') && (md[1] == '`') && (md[2] == '`'))
		return "-codeblock-";

	else {
		md = ubqt_markup_inline(md);
		md = ubqt_markup_whole_line(md);
	}

	return md;

}

char *
ubqt_markup_code(char *md)
{

	if ((md[0] == '`') && (md[1] == '`') && (md[2] == '`'))
		return "-codeblock-";

	else {
		md = ubqt_markup_line(md);
		asprintf(&md, "%s%s%s", "<span background=\"#444444\">", md, "</span>");
	}

	return md;

}
