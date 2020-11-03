#include <stdio.h>
#include "c9.h"
#include "mdns.h"
#include "microui.h"
#include "altid.h"

int
main(int argc, char* argv[])
{
	// Spin up our draw, keyboard, mouse
	// scanmdns will move to the main input switch, /scan, /connect will use
	struct DNSData* dns = NULL;
	scanmdns(dns);
}
