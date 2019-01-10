#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include "utf8.h"
#include "../../src/ubqt.h"

//TODO: Modes

int
ubqt_input_init() {
	/* Set up modes, cursor, default end of input if it exists */
	if (ubqt_win.input) {
		cursor.index = strlen(ubqt_win.input);
		cursor.cur = ubqt_win.input;
	}	
	else
		cursor.cur = NULL;
	
	cursor.index = 0;			
	return 0;
}

int
ubqt_input_handle(char *buffer) {

	char KEY_back[2], KEY_enter[2], KEY_escape[2];
	KEY_back[0] = '\x08';
	KEY_back[1] = '\x00';
	KEY_enter[0] = '\x0d';
	KEY_enter[1] = '\x00';
	KEY_escape[0] = '\x1b';
	KEY_escape[1] = '\x00';
	
	//TODO: if cursor.cur == NULL don't draw cursor
	if (!utf8cmp(KEY_escape, buffer)) {
		ubqt_win.input = "-exit-";
		return UBQT_FAILURE;
	}
	//TODO: ubqt prompt handling, readline mode as well as raw newline inserts on not-input window
	else if (!utf8cmp(KEY_enter, buffer)) {
		pthread_mutex_lock(&mutex);
		if(utf8cmp(ubqt_win.input, " ‣ ")) {
			ubqt_data_write("ctrl", ubqt_win.input + utf8size(" ‣ ") - 1);
			asprintf(&ubqt_win.input, "%s", " ‣ ");
		}
		pthread_mutex_unlock(&mutex);
		return UBQT_SUCCESS;
	}

	//TODO: backspace in normal mode handling? 
	else if (!utf8cmp(KEY_back, buffer)) {
		pthread_mutex_lock(&mutex);
		
		/* Ignore current character in backwards search */
		size_t i = utf8size(ubqt_win.input) - 1;

		/* 0x80 AND result means we are on continuation char */
		/* move back until we are on the start char          */
		while(i > 0 && (0x80 == (ubqt_win.input[i--] & 0xc0)));
		ubqt_win.input[i] = 0;
		pthread_mutex_unlock(&mutex);
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
	cursor.index = strlen(ubqt_win.input);	
	return UBQT_SUCCESS;
}
