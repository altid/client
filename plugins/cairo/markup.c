#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../src/ubqt.h"

char *
ubqt_markup_whole_line(char *md)
{

	if (md[0] == '#') {

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

	}

	/* TODO: Seems to miss on ubqt_markup_color, uncertain why */ 
	if (md[0] == '[' && md[1] == '#' && md[8] == ']') {

		char *color = strndup(md, strlen(md));
		ubqt_substr(color, 2, 6);

		asprintf(&md, "%s%s%s%s", "<span color=\"#", color, "\">", md + 9);

		free(color);
	}

	return md;

}

char *
ubqt_markup_inline(char *md)
{

	int i = 0;
	int len = strlen(md);
	
	/* sadly two loops for now, something is wrong */
	while(i < len) {
		switch(md[i]) {
			/*	
			case '&':
				ubqt_insert(md, "&amp;", i);
				i += 5;
				break;
			*/
			case '[':	
				/* [>] starts input */
				if (md[i + 1] == '>') {
					char *tmp = strndup(md, len);
					ubqt_substr(md, 0, i);
					asprintf(&md, "%s%s%s", md, "<span underline=\"low\"> ", tmp + i + 3);
					free(tmp);
					len = strlen(md);
					i += 23;

				}

				/* [#XXXXXX] starts color */
				else if (md[i + 1] == '#' && md[i + 8] == ']') {

					char *color = strndup(md, len);
					char *tmp = strndup(md, len);
					ubqt_substr(md, 0, i);
					ubqt_substr(color, i + 2, 6);

					asprintf(&md, "%s%s%s%s%s", md, "<span color=\"#", color, "\">", tmp + i + 9);

					free(tmp);
					free(color);

					len = strlen(md);

					/* Move i to end of newly added material */
					i += 21;

				}

				/* [#END] ends color or input*/
				else if (md[i + 2] == 'E' && md[i + 3] == 'N' && md[i + 4] == 'D' && md[i + 5] == ']') {
					char *tmp = strndup(md, len);

					ubqt_substr(md, 0, i);
					asprintf(&md, "%s%s%s", md, "</span>", tmp + i + 6);
					free(tmp);

					len = strlen(md);
					
					/* move i to end of newly added material */
					i += 6;
				} 

				i++;
				break;

			default:
				i++;
				break;
	
		}
	}

	return md;

}

char *
ubqt_markup_line(char *md)
{

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
