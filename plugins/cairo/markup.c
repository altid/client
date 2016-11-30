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

	return md;

}

char *
ubqt_markup_color(char *md) {

	int i = 0, len = strlen(md);

	while(i < len) {
		
		/* [#XXXXXX] starts color */
		if(len > (i + 9) && md[i] == '[' && md[i + 1] == '#' && md[i + 8] == ']') {
			char *tmp = strndup(md, len);
			char *color = strndup(md, len);

			ubqt_substr(md, 0, i);
			ubqt_substr(color, i + 2, 6);

			asprintf(&md, "%s%s%s%s%s", md, "<span color=\"#", color, "\">", tmp + i + 9);

			free(tmp);
			free(color);

			len = strlen(md);
		}

		/* [#END] ends color */
		else if(len > (i + 6) && md[i] == '[' && md[i + 2] == 'E' && md[i + 3] == 'N' && md[i + 4] == 'D' && md[i + 5] == ']') {
			char *tmp = strdup(md);

			ubqt_substr(md, 0, i);
			asprintf(&md, "%s%s%s", md, "</span>", tmp + i + 6);

			free(tmp);

			len = strlen(md);
		}

		else {
			i++;
		}
	}

	return md;

}

char *
ubqt_markup_inline(char *md)
{

	int i = 0;
	int len = strlen(md);

	while(i < len) {
		/* pointer to a pointer to my data, md */
		switch(md[i]) {

			case '&':
				ubqt_insert(md, "&amp;", i);
				len += 4;
				i += 4;
				break;

			case '<':
				ubqt_insert(md, "&lt;", i);
				len += 3;
				i += 3;
				break;

			case '>':
				ubqt_insert(md, "&gt;", i);
				len += 3;
				i += 3;
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
		md = ubqt_markup_color(md);
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
