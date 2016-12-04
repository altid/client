#define _GNU_SOURCE
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <errno.h>
#include "../../src/ubqt.h"

int
ubqt_data_init(char *path)
{
	ubqt_win.path = path;
	DIR *d;
	struct dirent *dp;

	if ((d = opendir(ubqt_win.path)) == NULL)
		return errno;

	while ((dp = readdir(d)) != NULL)
		ubqt_data_update(dp->d_name, ubqt_win.path);

	closedir(d);

	return 0;
}

int
ubqt_data_write(char *name, char *buffer)
{

	FILE *fp;
	char *fullpath = NULL;

	asprintf(&fullpath, "%s/%s", ubqt_win.path, name); 
	if ((fp = fopen(fullpath, "w")) == NULL)
		return UBQT_FAILURE;

	else
		fprintf(fp, "%s%s", buffer, "\n");
	
	fclose(fp);
	
	free(fullpath);

	return UBQT_SUCCESS;
}

char *
ubqt_data_read(char *name, char *path)
{

	FILE *fp;
	char *fullpath = NULL;
	char *markup = NULL;

	asprintf(&fullpath, "%s/%s", path, name);

	if ((fp = fopen(fullpath, "r")) == NULL) {
		fprintf(stderr, "Failed to open path %s\n", fullpath);
		free(fullpath);
		return NULL;
	}

	else {
		unsigned lineno = 0;
		size_t len = 0;
		ssize_t read;
		
		char *ln = NULL;
		bool codeblock = false;

		while((read = getline(&ln, &len, fp)) != -1) {

			if (codeblock)
				ln = ubqt_markup_code(ln);

			else
				ln = ubqt_markup_line(ln, lineno);

			/* Skip this line completely */
			if (!strcmp(ln, "-codeblock-"))
				codeblock = !codeblock;

			/* First line */
			else if(markup == NULL) {
				asprintf(&markup, "%s", ln);
				lineno++;
			}

			/* Next lines */
			else {
				asprintf(&markup, "%s%s", markup, ln);
				lineno++;
			}

		}
	}

	free(fullpath);

	return markup;

}
