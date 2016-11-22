#include <stdio.h>
#include <stdlib.h>

#include "ubqt.h"

int
main(int argc, char* argv[])
{

	if (argc != 2) {
		fprintf(stderr, "usage: %s <path>\n", argv[0]);
		return 2;
	}

	Win *win = {NULL};

	ubqt_connection_init(argv[1]);

	ubqt_window_init();

	ubqt_run_loop();

	ubqt_destroy();

	return 0;
}
