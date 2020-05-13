#include <u.h>
#include <libc.h>
#include <bio.h>
#include <stdio.h>

#include "altid.h"

/* TODO: Add a struct with channel, argc, and argv 
   we want to watch all paths sent in
   so we can update tabs, etc
*/

void
eventsthread(void *v)
{
	Channel *c;
	Event *event;
	Biobuf *bev;
	char *f, *path = nil;
	c = v;
	/* TODO: iterate through argv passed in v */
	snprintf(path, 80, "%s/events", "test");
	bev = Bopen(path, OREAD);
	if(bev == nil)
		sysfatal("Unable to open events file");
	while((f = Brdstr(bev, '\n', 1)) != nil){
		event = malloc(sizeof(Event));
		event->msg = f;
		event->msize = sizeof(f);
		send(c, &event);
	}
}

int
md_init_data(char *path, Data **d){
	USED(d);
	Channel *e;
	Dir *h;
	int dp;
	long n;

	dp = open(path, OREAD);
	if (dp < 0)
		sysfatal("Opening dir failed");
	n = dirreadall(dp, &h);
	close(dp);
	if(n < 0)
		sysfatal("Dirreadall");
	for(int i = 0; i < n; i++){
		if(strcmp(h[i].name, "tabs") == 0)
			print("tabs");
		/*switch(h[i].name){
		case "aside":
			print("aside");
		case "tabs":
			print("tabs");
		}*/
	}
	free(h);
	e = chancreate(sizeof(Event*), 0);
	threadcreate(eventsthread, e, 8192);
	return 1;
}

Channel *
md_tail_events(char *path) {
	USED(path);
	
}