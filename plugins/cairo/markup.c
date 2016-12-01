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

		default:
			break;
	}

	return md;

}

char *
ubqt_markup_inline(char *md)
{

	int i = 0;
	int len = strlen(md);
	char *tmp = NULL;

	while(i < len) {

		switch(md[i]) {
			/* If we're escaping something, cut slash, and move 2 forward */
			case '\\':
				tmp = strndup(md, len);
				ubqt_substr(md, 0, i);
				asprintf(&md, "%s%s", md, tmp + (++i));
				i++;
				break;

			//case '*':
				//if (md[1 + 1] == '*') {
					/* turn on or off span */
					//if (tag_open.strong_em);
						
				//}
			//	break;

			case '[':
				/* [>] starts input */
				if (md[i + 1] == 'i' && md[i + 2] == ']') {
					tmp = strndup(md, len);
					ubqt_substr(md, 0, i);
					asprintf(&md, "%s%s%s", md, "<span underline=\"low\"> ", tmp + i + 3);
					len = strlen(md);
					i += 23;

				}

				/* [#XXXXXX] starts color */
				else if (md[i + 1] == '#' && md[i + 8] == ']') {

					char *color = strndup(md, len);
					tmp = strndup(md, len);
					ubqt_substr(md, 0, i);
					ubqt_substr(color, i + 2, 6);

					asprintf(&md, "%s%s%s%s%s", md, "<span color=\"#", color, "\">", tmp + i + 9);

					free(color);

					len = strlen(md);
					i += 21;

				}

				/* [E] ends tags or input*/
				else if (md[i + 1] == 'E' && md[i + 2] == ']') {
					tmp = strndup(md, len);

					ubqt_substr(md, 0, i);
					asprintf(&md, "%s%s%s", md, "</span>", tmp + i + 3);

					len = strlen(md);

					/* move i to end of newly added material */
					i += 6;
				}

				i++;
				break;

			//case '\n':
			//case '\0':
				/* iterate through tags, close any open */
			//	break;

			default:
				i++;
				break;
		}
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
