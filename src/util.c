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

	else if (!strcmp(data, "tabs")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);
		ubqt_win.tabs = tmp;
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

	else if (!strcmp(data, "tabs")) {
		pthread_mutex_lock(&mutex);
		ubqt_win.tabs = NULL;
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

int
ubqt_join(char *first, char *second)
{

	char *new_str;
	asprintf(&new_str, "%s%s", first, second);
	free(first);
	return UBQT_SUCCESS;

}

int
ubqt_substr(char *md, unsigned start, unsigned end)
{


	unsigned i;
	char *str = malloc(end - start);

	for(i = start; i < end; i++)
		str[i - start] = md[i];

	str[end] = 0;

	return UBQT_SUCCESS;

}
