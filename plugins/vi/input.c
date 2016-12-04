#define _GNU_SOURCE
#include "stdio.h"
#include "utf8.h"
#include "../../src/ubqt.h"

int
ubqt_input_init()
{

	return 0;

}

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

	
	if (!utf8cmp(KEY_escape, buffer))
		return 1;

	else if (!utf8cmp(KEY_enter, buffer)) {
		pthread_mutex_lock(&mutex);
		if(utf8cmp(ubqt_win.input, " ‣ "))
			ubqt_data_write("input", ubqt_win.input + utf8size(" ‣ ") - 1);

		asprintf(&ubqt_win.input, "%s", " ‣ ");
		pthread_mutex_unlock(&mutex);
		return 0;
	}

	else if (!utf8cmp(KEY_back, buffer)) {
		/* move back through all continuation bits then null terminate */
		int i   = utf8size(ubqt_win.input) - 1;
		//TODO: int lim = utf8size(win.prompt);
		int lim = utf8size(" ‣ ");

		while(i >= lim && (0x80 == (ubqt_win.input[i--] & 0xc0)));
		ubqt_win.input[i] = 0;

		return 0;
	}

	else {
		pthread_mutex_lock(&mutex);
		if (ubqt_win.input)
			asprintf(&ubqt_win.input, "%s%s", ubqt_win.input, buffer);

		else
			asprintf(&ubqt_win.input, "%s", buffer);
		pthread_mutex_unlock(&mutex);
	}
	
	return 0;

}
