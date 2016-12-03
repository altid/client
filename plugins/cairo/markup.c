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

			else if (md[1] == '*' || md[1] == '-')
				asprintf(&md, "%s%s", " • ", md + 2);

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
ubqt_markup_inline(char *md, unsigned lineno)
{

	int i = 0;
	int len = strlen(md);
	char *tmp = NULL;

	while(i < len) {
		switch(md[i]) {

			/* If we're escaping something, cut slash, and move 2 forward */
			case '\\':
				i += ubqt_replace_ch(&md, md[i + 1], i, 2);
				len = strlen(md);
				break;

			case '*':
				if (!tag_open.ex_bold && md[i + 1] == '*') {
					tag_open.ex_bold = true;
					i += ubqt_replace(&md, "<span weight=\"ultrabold\">", i, 2);
				}

				else if (tag_open.ex_bold && md[i + 1] == '*') {
					tag_open.ex_bold = false;
					i += ubqt_replace(&md, "</span>", i, 2);
				}

				else if (!tag_open.bold) {
					tag_open.bold = true;
					i += ubqt_replace(&md, "<b>", i, 1);
				}

				else if (tag_open.bold) {
					tag_open.bold = false;
					i += ubqt_replace(&md, "</b>", i, 1);
				}

				/* We should never get here */
				else {
					i++;
				}

				len = strlen(md);
				break;

			case '_':
				if (!tag_open.ex_em && md[i + 1] == '_') {
					tag_open.ex_em = true;
					i += ubqt_replace(&md, "<span style=\"oblique\">", i, 2);
				}

				else if (tag_open.ex_em && md[i + 1] == '_') {
					tag_open.ex_em = false;
					i += ubqt_replace(&md, "</span>", i, 2);
				}

				else if (!tag_open.em) {
					tag_open.em = true;
					i += ubqt_replace(&md, "<i>", i, 1);
				}

				else if (tag_open.em) {
					tag_open.em = false;
					i += ubqt_replace(&md, "</i>", i, 1);
				}

				else
					i++;

				break;

			case '`':
				if (!tag_open.code) {
					tag_open.code = true;
					i += ubqt_replace(&md, "<span background=\"#444444\"><tt>", i, 1);
				}

				else if (tag_open.code) {
					tag_open.code = false;
					i += ubqt_replace(&md, "</tt></span>", i, 1);
				}

				else
					i++;
				break;

			case '~':
				if (!tag_open.strike && md[i + 1] == '~') {
					tag_open.strike = true;
					i += ubqt_replace(&md, "<s>", i, 2);
				}

				else if (tag_open.strike && md[i + 1] == '~') {
					tag_open.strike = false;
					i += ubqt_replace(&md, "</s>", i, 2);
				}

				else
					i++;
				break;

			case '[':

				/* [>myfile](Hint) sets up input block using as much line as possible */
				if (md[i + 1] == '>') {
					char *hint = strndup(md, len);
					tag_open.inp = true;

					//Locate closing of tag
					int j = ubqt_next(md, ']', i);
					ubqt_substr(hint, i + 2, j - 2);
					i += ubqt_replace(&md, hint, i, 2);

					i += ubqt_replace(&md, ": <u>", i, j);
					len = strlen(md);
					free(hint);
				}

				else if (md[i + 1] == '#' && md[i + 8] == ']') {
					tag_open.color = true;

					i += ubqt_replace(&md, "<span color=\"#", i, 2);
					i += ubqt_replace(&md, "\">", i + 6, 2);

					len = strlen(md);
				}

				else if (md[i + 2] == ']' )
					i += 3;

				else
					i++;

				break;
/*
			case '!':
				if(md[i + 1] == '[') {
					tag_open.image = true;
					tag_open.img[-1] = malloc(sizeof(tag_open.img[-1]));
					tag_open.img[-1].index = i + 2;
					i++;
				}

				i++;
				break;
*/
			case ')':
				if (tag_open.color) {
					i += ubqt_replace(&md, "</span>", i, 1);
					len = strlen(md);
				}

				else if (tag_open.inp) {
					i += ubqt_replace(&md, "</u>", i, 1);
					len = strlen(md);
				}

				if (tag_open.color)
					tag_open.color = false;

				else if (tag_open.inp)
					tag_open.inp = false;

				else
					i++;

				break;
/*
			case ']':

				if (tag_open.input && md[i + 1] == '(') {
					i += ubqt_replace(&md, "<span underline=\"low\"> ", i, 1);
					len = strlen(md);
				}

				else if (tag_open.image && md[i + 1] == '(') {
					tmp = strndup(md, len);
					ubqt_substr(tmp, tag_open.img[-1].index, i - 1 - tag_open.img[-1].index);
				*/	/* ![name](/path/to/image) */
					// tag_open.img[-1].key = name;
					// tag_open.img[-1].value
					// put the path here, so we can index it, parse the text by index, and profit.

				//}
					/* [name](http://somelink.com) */
				//else if (tag_open.path) {
						// tag_open.img[-1].str = path;
						// tag_open.img[-1].name = name;
						// may need to malloc?
				//}

			//	else
			//		i++;

			//	break;

			case '\n':
			case '\0':
				i = len;
				break;

			default:
				i++;
				break;
		}
	}

	free(tmp);
	return md;

}

char *
ubqt_markup_line(char *md, unsigned lineno)
{

	tag_open.ex_bold	= false;
	tag_open.bold		= false;
	tag_open.strike		= false;
	tag_open.square		= false;
	tag_open.ex_em		= false;
	tag_open.color		= false;
	tag_open.code		= false;
	tag_open.path		= false;
	tag_open.img 		= false;
	tag_open.inp		= false;
	tag_open.lnk		= false;
	tag_open.em			= false;

	if ((md[0] == '`') && (md[1] == '`') && (md[2] == '`')) {
		asprintf(&md, "%s", "-codeblock-");
	}

	else {
		md = ubqt_markup_whole_line(md);
		md = ubqt_markup_inline(md, lineno);
	}

	return md;

}

char *
ubqt_markup_code(char *md)
{

	if ((md[0] == '`') && (md[1] == '`') && (md[2] == '`'))
		asprintf(&md, "%s", "-codeblock-");

	else {
		asprintf(&md, "%s%s%s", "<span background=\"#444444\"><tt>", md, "</tt></span>");
	}

	return md;

}
