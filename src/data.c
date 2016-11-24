#include "ubqt.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

void
ubqt_data_update(char *data) {
		/* match name, update data */
		if (!strcmp(data, "text"))
				ubqt_win.text = ubqt_file_read(data);
		else if (!strcmp(data, "tabbar"))
				ubqt_win.tabbar = ubqt_file_read(data);
		else if (!strcmp(data, "title"))
				ubqt_win.title = ubqt_file_read(data);
		else if (!strcmp(data, "status"))
				ubqt_win.status = ubqt_file_read(data);
		else if (!strcmp(data, "sidebar"))
				ubqt_win.sidebar = ubqt_file_read(data);
		else if (!strcmp(data, "slideout"))
				ubqt_win.slideout = ubqt_file_read(data);
		
}

void
ubqt_data_remove(char *data) {
		if (!strcmp(data, "title"))
				ubqt_win.title = NULL;
		else if (!strcmp(data, "tabbar"))
				ubqt_win.tabbar = NULL;
		else if (!strcmp(data, "status"))
				ubqt_win.status = NULL;
		else if (!strcmp(data, "sidebar"))
				ubqt_win.sidebar = NULL;
		else if (!strcmp(data, "slideout"))
				ubqt_win.sidebar = NULL;
}

int
ubqt_data_init(const char *path) {
		DIR *d;
		struct dirent *dir;
		d = opendir(path);
		if(!d)
				return 0;
		while ((dir = readdir(d)) != NULL)
				ubqt_data_update(dir->d_name);
		closedir(d);

		return 1;
}

char *
ubqt_file_read(char *name) {
		char *buf = NULL;
		FILE *fp;
		int str_sz;
		char *fullpath = malloc(strlen(name) + strlen(path) + 2);
		sprintf(fullpath, "%s/%s", path, name);	
		fp = fopen(fullpath, "r");
		free(fullpath);
		if (fp) {
				fseek(fp, 0, SEEK_END);
				str_sz = ftell(fp);
				rewind(fp);
				buf = (char*) malloc(sizeof(char) * (str_sz + 1));
				fread(buf, sizeof(char), str_sz, fp);
				buf[str_sz] = '\0';
				fclose(fp);
		}
		
		return (buf) ? buf : NULL;
}
