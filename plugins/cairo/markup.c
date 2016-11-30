#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../src/ubqt.h"

char *
ubqt_markup_whole_line(char **md)
{

	if (*md[0] == '#') {

		if(*md[1] != '#')
			asprintf(md, "%s%s%s", "<span size=\"xx-large\" weight=\"bold\">", *md, "</span>");

		else if(*md[2] != '#')
			asprintf(md, "%s%s%s", "<span size=\"x-large\" weight=\"bold\">",  *md, "</span>");

		else if(*md[3] != '#')
			asprintf(md, "%s%s%s", "<span size=\"large\" weight=\"bold\">", *md, "</span>");

		else if(*md[4] != '#')
			asprintf(md, "%s%s%s", "<span size=\"medium\" weight=\"bold\">", *md, "</span>");

		else if(*md[5] != '#')
			asprintf(md, "%s%s%s", "<span size=\"small\" weight=\"bold\">", *md, "</span>");

		else
			asprintf(md, "%s%s%s", "<span size=\"x-small\" weight=\"bold\">", *md, "</span>");

	}

	if ((*md[0] == '`') && (*md[1] == '`') && (*md[2] == '`'))
		ubqt_win.iscode = true;

	return *md;

}

char *
ubqt_markup_color(char **md) {

	int i = 0, len = strlen(*md);
	char *tmp = NULL;
	char *foo = *md;

	while(i < len) {
		
		/* [#XXXXXX] starts color */
		if(len > (i + 9) && foo[i] == '[' && foo[i + 1] == '#' && foo[i + 8] == ']') {
			tmp = strndup(foo, len);
			char *color = strndup(foo, len);

			ubqt_substr(&foo, 0, i);
			ubqt_substr(&color, i + 2, 6);

			asprintf(&foo, "%s%s%s%s%s", foo, "<span color=\"#", color, "\">", tmp + i + 9);

			free(tmp);
			free(color);

			len = strlen(foo);
		}

		/* [#END] ends color */
		else if(len > (i + 6) && foo[i] == '[' && foo[i + 2] == 'E' && foo[i + 3] == 'N' && foo[i + 4] == 'D' && foo[i + 5] == ']') {
			tmp = strdup(foo);

			ubqt_substr(&foo, 0, i);
			asprintf(&foo, "%s%s%s", foo, "</span>", tmp + i + 6);

			free(tmp);

			len = strlen(foo);
		}

		else {
			i++;
		}
	}

	strncpy(*md, foo, strlen(foo));

	return *md;

}

char *
ubqt_markup_inline(char **md)
{

	int i = 0;
	char *foo = *md;
	int len = strlen(*md);

	while(i < len) {
		/* pointer to a pointer to my data, md */
		switch(foo[i]) {

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

	return *md;

}

char *
ubqt_markup_line(char **md)
{
	
	ubqt_markup_inline(md);
	printf("markup here %s\n", *md);
	ubqt_markup_color(md);
	printf("markup here %s\n", *md);
	ubqt_markup_whole_line(md);
	printf("markup here %s\n", *md);

	return *md;
}

char *
ubqt_markup_code(char **md)
{

	if ((*md[0] == '`') && (*md[1] == '`') && (*md[2] == '`')) {
		return "-codeblock-";
	}

	asprintf(md, "%s%s%s", "<span background=\"#444444\">", ubqt_markup_line(md), "</span>");

	return *md;

}
