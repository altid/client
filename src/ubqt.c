#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "ubqt.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *
run_file_loop(void *arg)
{

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
		fprintf(stderr, "Window init error: %s\n", strerror(err));

	else if ((err = pthread_mutex_init(&mutex, NULL)))
			fprintf(stderr, "Unable to create mutex: %s\n", strerror(err));

	else if ((err = pthread_create(&file_thread, NULL, run_file_loop, argv[1])))
			//TODO: Attempt to reconnect on failure
			fprintf(stderr, "I/O thread init failure: %s\n", strerror(err));

	else {
		/* If we make it to here, all init succeeded; run main thread */
		ubqt_main_loop();

		/* Clean up */
		if ((err = pthread_mutex_destroy(&mutex)))
			fprintf(stderr, "Unable to destroy mutex: %s\n", strerror(err));

		if ((err = pthread_cancel(file_thread)))
			fprintf(stderr, "I/O thread cleanup failure: %s\n", strerror(err));

		if ((err = ubqt_destroy()))
			fprintf(stderr, "Error cleaning up backend: %s\n", strerror(err));

		if (!err)
			/* If cleanup succeeds, drop good exit code */
			exit(EXIT_SUCCESS);

	}

	exit(EXIT_FAILURE);

}
