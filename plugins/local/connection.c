#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "../../src/ubqt.h"

int
ubqt_data_init(char *path)
{
	DIR *d;
	struct dirent *dp;

	if ((d = opendir(path)) == NULL)
		return errno;

	while ((dp = readdir(d)) != NULL)
		ubqt_data_update(dp->d_name, path);

	closedir(d);

	return 0;
}

char *
ubqt_data_read(char *name, char *path)
{

	FILE *fp;
	char *fullpath;

	fullpath = malloc(strlen(name) + strlen(path) + 2);

	if (!fullpath) {
		fprintf(stderr, "Failed to read %s\n", path);
		return NULL;
	}

	if ((sprintf(fullpath, "%s/%s", path, name)) < 0)
		fprintf(stderr, "Failed to build path %s/%s\n", path, name);

	fullpath[strlen(fullpath)] = 0;

	if ((fp = fopen(fullpath, "r")) == NULL) {
		fprintf(stderr, "Failed to open path %s\n", fullpath);
		free(fullpath);
		return NULL;
	}

	else {
		size_t len = 0;
		ssize_t read;
		char *ln, *str, *tmp, *markup = "";
		bool codeblock = false;

		while((read = getline(&ln, &len, fp)) != 1) {

			/* Hold previous result */
			tmp = markup;

			if (codeblock)
				str = ubqt_markup_code(ln);
			
			else
				str = ubqt_markup_line(ln);

			/* Set flag only, no append */
			if (!strcmp(str, "-codeblock-")) {
				if (codeblock)
					codeblock = false;
				else
					codeblock = true;
				continue;
			}

			markup = ubqt_join(tmp, str);

		}

		if (read != -1)
			free(ln);

		free(fullpath);

		return markup;
	
	}

	return NULL;

}
