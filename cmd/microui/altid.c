#include <stdio.h>
#include "mdns.h"
#include "altid.h"

int
main(int argc, char* argv[])
{
	Service *services;
	pthread_t list;

	/* 
	 * Send off thread to populate service list
	 * this will be read whenever someone presses find service
	 * and optionally updated when they refresh
	 */
	services = malloc(sizeof(Service));
	pthread_mutex_init(&lock, NULL);
	pthread_create(&list, NULL, initmdns, &services);

	/* There is no default data connection, they're started in
	 * draw.c via UI. They run in a separate thread, and can call
	 * write_file in a threadsafe way. They then send an SDL_Event
	 * to issue a redraw
	 * Our main loop/thread then focuses on drawing, and responding to UI.
	 */
	draw_loop(services);
	freeservice(services);
	
	pthread_mutex_destroy(&lock);
}
