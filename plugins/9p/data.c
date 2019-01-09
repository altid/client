#define IXP_NO_P9_ //no prefix
#define IXP_P9_STRUCTS
#include <stdint.h>
#include <unistd.h>
#include <ixp.h>
#include "../../src/ubqt.h"

#define MAX_EVENTS 16
IxpClient *client;

int
ubqt_data_init(const char *path) {
	client = ixp_mount(path);
	if (!client)
		return 1;
	return 0;
}

int
ubqt_data_loop(char *path) {
	//read event, update based on string result from reads
	//ubqt_data_update(name, path)
	//ubqt_draw_new_data_callback()
	//ubqt_data_remove(name)
	//client.fd, msize, lastfid
	sleep(10000);
	ixp_unmount(client);
	return 0;
}

int
ubqt_data_write(char *name, char *buffer) {
	IxpCFid *fid;
	fid = ixp_open(client, name, P9_OWRITE);
	ixp_write(fid, buffer, sizeof(buffer));
	ixp_close(fid);
	return 0;
}

char *
ubqt_data_read(char *name, char *path) {
	IxpCFid *fid;
	char *file, *buf;
	int count;
	asprintf(&file, "%s/%s", name, path);
	fid = ixp_open(client, file, P9_OREAD);
	while((count = ixp_read(fid, buf, fid->iounit)) > 0)
		//TODO: allocate buffer here, grow as needed
		//Return newly allocated buffer
	return buf;
}

// TODO: ixp_remove(client, const char "notify");
