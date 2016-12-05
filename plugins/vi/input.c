#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include "utf8.h"
#include "../../src/ubqt.h"

int
ubqt_input_init()
{

	return 0;

}

/* Return UBQT_FAILURE on exit so we can exit */
int
ubqt_input_handle(char *buffer)
{

	char KEY_back[2], KEY_enter[2], KEY_escape[2];
	KEY_back[0] = '\x08';
	KEY_back[1] = '\x00';

	KEY_enter[0] = '\x0d';
	KEY_enter[1] = '\x00';

	KEY_escape[0] = '\x1b';
	KEY_escape[1] = '\x00';

	
	if (!utf8cmp(KEY_escape, buffer)) {
		ubqt_win.input = "-exit-";
		return UBQT_FAILURE;
	}

	else if (!utf8cmp(KEY_enter, buffer)) {
		pthread_mutex_lock(&mutex);

		if(utf8cmp(ubqt_win.input, " ‣ ")) {
			ubqt_data_write("input", ubqt_win.input + utf8size(" ‣ ") - 1);
			asprintf(&ubqt_win.input, "%s", " ‣ ");
		}

		pthread_mutex_unlock(&mutex);

		return UBQT_SUCCESS;
	}

	else if (!utf8cmp(KEY_back, buffer)) {
		
		/* Ignore current character in backwards search */
		size_t i = utf8size(ubqt_win.input) - 1;

		/* 0x80 AND result means we are on continuation */
		/* move back until we are on the start char     */
		while(i > 0 && (0x80 == (ubqt_win.input[i--] & 0xc0)));
		ubqt_win.input[i] = 0;

		return UBQT_SUCCESS;
	}

	else {
		pthread_mutex_lock(&mutex);
		if (ubqt_win.input)
			asprintf(&ubqt_win.input, "%s%s", ubqt_win.input, buffer);

		else
			asprintf(&ubqt_win.input, "%s", buffer);
		pthread_mutex_unlock(&mutex);
	}
	
	return UBQT_SUCCESS;

}
