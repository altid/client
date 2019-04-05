#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include "altid.h"

void
altid_data_update(char *data, char *path) {
	/*
	 * Ordered by presumed frequency
	 * file read errors will result in NULL value assignment
	 * lock mutex only after data has been parsed
	 * to avoid locking up gfx thread too much
	 * so markup operations can be as advanced as we want
	 */
	char *tmp = NULL;

	if (!strcmp(data, "feed")) {
		tmp = altid_data_read(data, path);
		pthread_mutex_lock(&mutex);
		if (altid_win.main != NULL)
			free(altid_win.main);
		altid_win.main = tmp;
		pthread_mutex_unlock(&mutex);
	}
	else if (!strcmp(data, "tabs")) {
		tmp = altid_data_read(data, path);
		pthread_mutex_lock(&mutex);
		if (altid_win.tabs != NULL)
			free(altid_win.tabs);
		altid_win.tabs = tmp;
		pthread_mutex_unlock(&mutex);
	}
	else if (!strcmp(data, "title")) {
		tmp = altid_data_read(data, path);
		pthread_mutex_lock(&mutex);
		if (altid_win.title != NULL)
			free(altid_win.title);
		altid_win.title = tmp;
		pthread_mutex_unlock(&mutex);
	}
	else if (!strcmp(data, "status")) {
		tmp = altid_data_read(data, path);
		pthread_mutex_lock(&mutex);
		if (altid_win.status != NULL)
			free(altid_win.status);
		altid_win.status = tmp;
		pthread_mutex_unlock(&mutex);
	}
	else if (!strcmp(data, "sidebar")) {
		tmp = altid_data_read(data, path);
		pthread_mutex_lock(&mutex);
		if (altid_win.sidebar != NULL)
			free(altid_win.sidebar);
		altid_win.sidebar = tmp;
		pthread_mutex_unlock(&mutex);
	}
	else if (!strcmp(data, "slideout")) {
		tmp = altid_data_read(data, path);
		pthread_mutex_lock(&mutex);
		if (altid_win.slideout != NULL)
			free(altid_win.slideout);
		altid_win.slideout = tmp; 
		pthread_mutex_unlock(&mutex);
	}
}

void
altid_data_remove(char *data) {
	pthread_mutex_lock(&mutex);

	/* Ordered by frequency */
	if (!strcmp(data, "title")) {
		if (altid_win.title != NULL)
			free(altid_win.title);
		altid_win.title = NULL;
	}
	else if (!strcmp(data, "tabs")) {
		if (ubqt_win.tabs != NULL)
			free(altid_win.tabs);
		altid_win.tabs = NULL;
	}
	else if (!strcmp(data, "status")) {
		if (altid_win.status != NULL)
			free(altid_win.status);
		altid_win.status = NULL;
	}
	else if (!strcmp(data, "sidebar")) {
		if (altid_win.sidebar != NULL)
			free(altid_win.sidebar);
		altid_win.sidebar = NULL;
	}
	else if (!strcmp(data, "slideout")) {
		if (altid_win.slideout != NULL)
			free(altid_win.slideout);
		altid_win.slideout = NULL;
	}
	else if (!strcmp(data, "feed")) {
		if (altid_win.main != NULL)
			free(altid_win.main);
		altid_win.main = NULL;
	}
	pthread_mutex_unlock(&mutex);
}

int
altid_substr(char *str, unsigned start, unsigned end)
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
	for(i = 0; i < end; i++) {
		str[i] = str[i + start];
	}

	str[end] = 0;
	return UBQT_SUCCESS;
}

int
altid_insert(char **str, const char *token, unsigned index) {

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
altid_replace(char **str, const char *token, unsigned index, unsigned range)
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
altid_replace_ch(char **str, const char token, unsigned index, unsigned range) {
	unsigned start = strlen(*str);

	if(range > start)
		return 0;

	char *tmp = strndup(*str, index);
	asprintf(str, "%s%c%s", tmp, token, *str + index + range);

	free(tmp);
	return strlen(*str) + range - start;
}

unsigned
altid_next(char *md, const char token, unsigned i) {
	if (i > strlen(md))
		return 0;
	for(; i < strlen(md); i++) {
		if(md[i] == token)
			return i;
	}
	return 0;
}
