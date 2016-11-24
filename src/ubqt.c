#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "ubqt.h"

void *
run_file_loop() {
		ubqt_file_loop();
		return NULL;
}

int
main(int argc, char* argv[])
{

	if (argc != 2) {
		fprintf(stderr, "usage: %s <path>\n", argv[0]);
		return 2;
	}

	ubqt_connection_init(argv[1]);
	
    if (!ubqt_data_init())
		fprintf(stderr, "Error reading from the files in %s\n", argv[1]);

	ubqt_window_init();

	pthread_t file_thread;
	pthread_create(&file_thread, NULL, run_file_loop, NULL);

	ubqt_main_loop();

	pthread_cancel(file_thread);
	ubqt_destroy();

	return 0;
}
