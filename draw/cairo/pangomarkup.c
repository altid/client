#include <string.h>
#include "pangomarkup.h"

void
parse_line(char *line)
{
	(void) line;
	//char *buf = 0;

	//<span size="xx-large">line</span>
	//if(!strncmp(line, "######", 6))
	//	sprintf(buf, "%s%s%s\n", "<span size=\"xx-large\">", line, "</span>");

}

char *
ubqt_toxml(char *md, int width, int height)
{

	(void) width; (void) height;
	size_t i;
	char line[4096];

	for(i = 0; i < strlen(md); i++) {
		line[i] = md[i];
		if(md[i] == '\n')
		parse_line(line);
	}

	return md;

}
