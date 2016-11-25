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

	if (!strcmp(data, "text")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.text = ubqt_file_read(data, path);
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "tabbar")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.tabbar = ubqt_file_read(data, path);
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "title")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.title = ubqt_file_read(data, path);
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "status")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.status = ubqt_file_read(data, path);
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "sidebar")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.sidebar = ubqt_file_read(data, path);
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "slideout")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.slideout = ubqt_file_read(data, path);
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

	else if (!strcmp(data, "text")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.text = NULL;
		pthread_mutex_unlock(&mutex);
	}

}

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

	return (errno) ? errno : 0;

}

char *
ubqt_file_read(char *name, char *path)
{

	char *buf = NULL;
	FILE *fp = NULL;
	int str_sz;
	char *fullpath;

	fullpath = malloc(strlen(name) + strlen(path) + 2);

	if (!fullpath) {
		fprintf(stderr, "Failed to read %s\n", path);
		return NULL;
	}

	else if ((sprintf(fullpath, "%s/%s", path, name)) < 0)
		fprintf(stderr, "Failed to build path %s/%s\n", path, name);

	fullpath[strlen(fullpath)] = '\0';

	if ((fp = fopen(fullpath, "r")) == NULL)
		fprintf(stderr, "Failed to open path %s\n", fullpath);

	else {
		//TODO: Read in line by line, parse markup
		fseek(fp, 0, SEEK_END);
		str_sz = ftell(fp);
		rewind(fp);
		buf = (char*) malloc(sizeof(char) * (str_sz + 1));
		fread(buf, sizeof(char), str_sz, fp);
		buf[str_sz] = '\0';
		fclose(fp);
	}

	free(fullpath);
	return buf;
}
