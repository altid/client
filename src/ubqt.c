#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "ubqt.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *
run_file_loop(void *arg)
{

	//TODO: Errmsg from threading
	char *path = arg;
	ubqt_file_loop(path);
	return 0;

}

int
main(int argc, char* argv[])
{

	int err;
	pthread_t file_thread;

	if (argc != 2)
		fprintf(stderr, "usage: %s <path>\n", argv[0]);

	else if ((err = ubqt_connection_init(argv[1])))
		fprintf(stderr, "Connection error: %s\n", strerror(err));

	else if ((err = ubqt_data_init(argv[1])))
		fprintf(stderr, "Data init error %s: %s\n", argv[1], strerror(err));

	else if ((err = ubqt_window_init(argv[1])))
		fprintf(stderr, "Error initializing window: %s\n", strerror(err));

	else {
		pthread_mutex_init(&mutex, NULL);
		if ((err = pthread_create(&file_thread, NULL, run_file_loop, argv[1])))
			//TODO: Attempt to reconnect on failure
			fprintf(stderr, "I/O thread failure: %s\n", strerror(err));

		ubqt_main_loop();
		pthread_mutex_destroy(&mutex);

		if ((err = pthread_cancel(file_thread)))
			fprintf(stderr, "I/O thread cleanup failure: %s\n", strerror(err));

		ubqt_destroy();
		exit(EXIT_SUCCESS);
	}

	exit(EXIT_FAILURE);

}
