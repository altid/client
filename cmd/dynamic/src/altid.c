#include <libdill.h>
#include <stdio.h>

#include "altid.h"

int main(int argc, char* argv[]) {

	// Initiate keyboard thread
	// Initiate drawing thread
	// /scan command should mDNS scan
	// /connect irc should work
	// everything on the command line, like `irc discord etc` connect to

	// Channels to main thread --> switch on everything
	struct DNSData* dns = NULL;
	scanmdns(dns);
}
