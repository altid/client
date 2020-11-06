#include <stdio.h>
#include "mdns.h"
#include "altid.h"

int
main(int argc, char* argv[])
{
	pthread_mutex_init(&lock, NULL);
	//draw_loop(inputchan);
	draw_loop();
	pthread_mutex_destroy(&lock);
}
