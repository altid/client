#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "ubqt.h"

void
ubqt_data_update(char *data, char *path)
{
	/*
	 * Ordered by presumed frequency
	 * file read errors will result in NULL value assignment
	 */
	char *tmp;

	if (!strcmp(data, "main")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.main = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "tabbar")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.tabbar = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "title")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.title = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "status")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.status = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "sidebar")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.sidebar = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "slideout")) {
		tmp = ubqt_data_read(data, path);
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
		ubqt_win.main = NULL;
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
ubqt_substr(char *md, int start, int end)
{

	if (end <= start)
		return NULL;

	int i;
	char *str = malloc(end - start);

	for(i = start; i < end; i++)
		str[i - start] = md[i];

	str[end] = 0;

	return str;

}
