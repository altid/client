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
	 * lock mutex only after data has been parsed
	 * to avoid locking up gfx thread too much
	 * so markup operations can be as advanced as we want
	 */

	char *tmp = NULL;

	if (!strcmp(data, "main")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);

		if (ubqt_win.main != NULL)
			free(ubqt_win.main);

		ubqt_win.main = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "tabs")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);

		if (ubqt_win.tabs != NULL)
			free(ubqt_win.tabs);

		ubqt_win.tabs = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "title")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);

		if (ubqt_win.title != NULL)
			free(ubqt_win.title);

		ubqt_win.title = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "status")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);

		if (ubqt_win.status != NULL)
			free(ubqt_win.status);

		ubqt_win.status = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "sidebar")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);

		if (ubqt_win.sidebar != NULL)
			free(ubqt_win.sidebar);

		ubqt_win.sidebar = tmp;
		pthread_mutex_unlock(&mutex);
	}

	else if (!strcmp(data, "slideout")) {
		tmp = ubqt_data_read(data, path);
		pthread_mutex_lock(&mutex);

		if (ubqt_win.slideout != NULL)
			free(ubqt_win.slideout);

		ubqt_win.slideout = tmp; 
		pthread_mutex_unlock(&mutex);
	}

}

void
ubqt_data_remove(char *data)
{

	pthread_mutex_lock(&mutex);

	/* Ordered by frequency */
	if (!strcmp(data, "title")) {
		if (ubqt_win.title != NULL)
			free(ubqt_win.title);

		ubqt_win.title = NULL;
	}

	else if (!strcmp(data, "tabs")) {
		if (ubqt_win.tabs != NULL)
			free(ubqt_win.tabs);

		ubqt_win.tabs = NULL;
	}

	else if (!strcmp(data, "status")) {
		if (ubqt_win.status != NULL)
			free(ubqt_win.status);

		ubqt_win.status = NULL;
	}

	else if (!strcmp(data, "sidebar")) {
		if (ubqt_win.sidebar != NULL)
			free(ubqt_win.sidebar);

		ubqt_win.sidebar = NULL;
	}

	else if (!strcmp(data, "slideout")) {
		if (ubqt_win.slideout != NULL)
			free(ubqt_win.slideout);

		ubqt_win.sidebar = NULL;
	}

	else if (!strcmp(data, "main")) {
		if (ubqt_win.main != NULL)
			free(ubqt_win.main);

		ubqt_win.main = NULL;
	}

	pthread_mutex_unlock(&mutex);

}

int
ubqt_substr(char *str, unsigned start, unsigned end)
{

	/* If we have a string that is 4 chars long, and
	 * we're trying to get substring 2, 5, we'll be
	 * looking 3 chars past the end of our string
	 * make sure we fail here
	 */
	if (str == NULL)
		return UBQT_FAILURE;

	if (start + end > strlen(str))
		return UBQT_FAILURE;

	/* this would return a 0 length string */
	if (end == 0) {
		str[0] = 0;
		return UBQT_SUCCESS;
	}

	unsigned i;

	for(i = 0; i < end; i++)
		str[i] = str[i + start];

	str[end] = 0;

	return UBQT_SUCCESS;

}

int
ubqt_insert(char **str, const char *token, unsigned index)
{

	/* Out of bounds */
	if (str == NULL)
		return 0;

	if(index > strlen(*str))
		return 0;

	unsigned start = strlen(*str);
	char *tmp = strndup(*str, index);

	asprintf(str, "%s%s%s", tmp, token, *str + index);
	free(tmp);

	return strlen(*str) - start;

}

int
ubqt_replace(char **str, const char *token, unsigned index, unsigned range)
{

	unsigned start = strlen(*str);
	
	/* Out of bounds */
	if(index > start || range > start)
		return 0;

	if(index + range > strlen(*str))
		return 0;

	char *tmp = strndup(*str, index);

	asprintf(str, "%s%s%s", tmp, token, *str + index + range);
	free(tmp);

	return strlen(*str) + range - start;

}

int
ubqt_replace_ch(char **str, const char token, unsigned index, unsigned range)
{

	unsigned start = strlen(*str);

	if(range > start)
		return 0;

	char *tmp = strndup(*str, index);

	asprintf(str, "%s%c%s", tmp, token, *str + index + range);
	free(tmp);

	return strlen(*str) + range - start;
}

unsigned
ubqt_next(char *md, const char token, unsigned i)
{

	if (i > strlen(md))
		return 0;

	for(; i < strlen(md); i++) {
		if(md[i] == token)
			return i;
	}

	return 0;

}
