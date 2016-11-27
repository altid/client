#define _GNU_SOURCE
#include "ubqt.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

char *ubqt_file_read(char *, char *);

void
ubqt_data_update(char *data, char *path)
{
	/*
	 * Ordered by presumed frequency
	 * file read errors will result in NULL value assignment
	 */
	char *tmp;

	if (!strcmp(data, "main")) {
		tmp = ubqt_file_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.text = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "tabbar")) {
		tmp = ubqt_file_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.tabbar = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "title")) {
		tmp = ubqt_file_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.title = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "status")) {
		tmp = ubqt_file_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.status = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "sidebar")) {
		tmp = ubqt_file_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.sidebar = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "slideout")) {
		tmp = ubqt_file_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.slideout = tmp; 
		pthread_mutex_unlock(&mutex);
	}

}

void
ubqt_data_remove(char *data)
{

	/* Ordered by frequency */
	if (!strcmp(data, "title")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.title = NULL;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "tabbar")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.tabbar = NULL;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "status")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.status = NULL;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "sidebar")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.sidebar = NULL;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "slideout")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.sidebar = NULL;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "main")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.text = NULL;
		pthread_mutex_unlock(&mutex);
	}

}

char *
ubqt_join(char *first, char *second)
{

	char *new_str;
	asprintf(&new_str, "%s%s", first, second);

	return new_str;

}

char *
ubqt_file_read(char *name, char *path)
{

	FILE *fp;
	char *fullpath;

	fullpath = malloc(strlen(name) + strlen(path) + 2);

	if (!fullpath) {
		fprintf(stderr, "Failed to read %s\n", path);
		return NULL;
	}

	else if ((sprintf(fullpath, "%s/%s", path, name)) < 0)
		fprintf(stderr, "Failed to build path %s/%s\n", path, name);

	fullpath[strlen(fullpath)] = '\0';

	if ((fp = fopen(fullpath, "r")) == NULL) {
		fprintf(stderr, "Failed to open path %s\n", fullpath);
		free(fullpath);
		return NULL;
	}

	else {
		size_t len = 0;
		ssize_t read;
		char *ln;
		char *markup = "";
		char *str;
		char *tmp;

		while((read = getline(&ln, &len, fp)) != -1) {
			
			/* Hold previous result */
			tmp = markup;

			if (tag.codeblk)
				str = ubqt_markup_code(ln);
			else
				str = ubqt_markup_line(ln);

			/* set flag only */
			if (!strcmp(str, "-codeblock-")) {
				if (tag.codeblk)
					tag.codeblk = false;
				else
					tag.codeblk = true;
				continue;
			}


			markup = ubqt_join(tmp, str);

		}

		if(read != -1)
			free(ln);

		free(fullpath);

		return markup;

	}

	return NULL;

}

int
ubqt_data_init(char *path)
{

	DIR *d;
	struct dirent *dp;

	if ((d = opendir(path)) == NULL)
		return errno;

	while ((dp = readdir(d)) != NULL) {
		ubqt_data_update(dp->d_name, path);
	}

	closedir(d);

	return 0;

}
