#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../../src/ubqt.h"


char *
ubqt_markup_whole_line(char *md)
{

	char *markup;
	char *tmp = md;

	if (md[0] == '#') {

		if(md[1] != '#')
			markup = ubqt_join("<span size=\"xx-large\" weight=\"bold\">", md+=1);

		else if(md[2] != '#')
			markup = ubqt_join("<span size=\"x-large\" weight=\"bold\">",  md+=2);

		else if(md[3] != '#')
			markup = ubqt_join("<span size=\"large\" weight=\"bold\">", md+=3);

		else if(md[4] != '#')
			markup = ubqt_join("<span size=\"medium\" weight=\"bold\">", md+=4);

		else if(md[5] != '#')
			markup = ubqt_join("<span size=\"small\" weight=\"bold\">", md+=5);

		else
			markup = ubqt_join("<span size=\"x-small\" weight=\"bold\">", md+=6);

		tmp = markup;
		tmp[strlen(tmp) - 1] = 0;
		markup = ubqt_join(tmp, "</span>");
		return markup;
	}

	if ((md[0] == '`') && (md[1] == '`') && (md[2] == '`'))
		return "-codeblock-";

	return md;

}

char *
ubqt_markup_color(char *md) {

	int i = 0, len = strlen(md);
	char *m = md;

	while(i < len) {

		/* [#XXXXXX] starts color */
		if(len > (i + 9) && m[i] == '[' && m[i + 1] == '#' && m[i + 8] == ']') {
			char *color;
			char *tmp;
			char *tmp2;

			if (i > 0)
				tmp = ubqt_join(ubqt_substr(m, 0, i), "<span color=\"#");

			else
				tmp = "<span color=\"#";

			color = ubqt_substr(m, i + 2, i + 8);
			tmp2 = ubqt_join(tmp, color);

			tmp = ubqt_join(tmp2, "\">");
			m = ubqt_join(tmp, m+=(i + 9));
			len = strlen(m);
			i = 0;
		}

		/* [#END] ends color */
		else if(m[i] == '[' && m[i + 2] == 'E' && m[i + 3] == 'N' && m[i + 4] == 'D' && m[i + 5] == ']') {
			char *tmp;
			char *tmp2;
			tmp = ubqt_join(ubqt_substr(m, 0, i), "</span>");
			tmp2 = ubqt_join(tmp, m+=(i + 6));
			m = tmp2;
			len = strlen(m);
			i = 0;
		}

		else {
			i++;
		}
	}

	return m;

}

char *
ubqt_markup_inline(char *md)
{

	int i, len = strlen(md);
	char *tmp = md;
	char *markup = md;

	for(i = 0; i < len; i++) {
		switch(md[i]) {	
		case '&':
			markup = tmp;
			tmp = ubqt_join(ubqt_substr(markup, 0, i), "&amp; ");
			markup = ubqt_join(tmp, md+=(i + 1));
			tmp = markup;
			break;
		case '<':
			markup = tmp;
			tmp = ubqt_join(ubqt_substr(markup, 0, i), "&lt; ");
			markup = ubqt_join(tmp, md+=(i + 1));
			tmp = markup;
			break;
		case '>':
			markup = tmp;
			tmp = ubqt_join(ubqt_substr(markup, 0, i), "&gt; ");
			markup = ubqt_join(tmp, md+=(i + 1));
			tmp = markup;
			break;
		}

	}
	return markup;

}

char *
ubqt_markup_line(char *md)
{

	char *markup;
	char *tmp;

	markup = ubqt_markup_inline(md);

	tmp = ubqt_markup_color(markup);

	markup = ubqt_markup_whole_line(tmp);

	return markup;

}

char *
ubqt_markup_code(char *md) {

	if ((md[0] == '`') && (md[1] == '`') && (md[2] == '`'))
		return "-codeblock-";

	char *markup, *tmp;

	tmp = ubqt_markup_color(md);

	markup = ubqt_join("<span background=\"#444444\">", tmp);

	tmp = markup;
	tmp[strlen(tmp) - 1] = '\0';

	markup = ubqt_join(tmp, "</span>");

	return markup;

}
