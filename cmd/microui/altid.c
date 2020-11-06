#include <stdio.h>
#include "mdns.h"
#include "draw.h"
#include "altid.h"

int
main(int argc, char* argv[])
{
	//draw_loop(inputchan);
	draw_loop();
	// scanmdns will move to the main input switch, /scan, /connect will use
	struct DNSData* dns = NULL;
	scanmdns(dns);
}
