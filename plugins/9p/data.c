#define IXP_NO_P9_ //no prefix
#define IXP_P9_STRUCTS
#include <stdint.h>
#include <ixp.h>
#include "../../src/ubqt.h"

#define MAX_EVENTS 16

static IxpClient *client;

int
ubqt_data_init(char *path)
{

	//tcp!192.168.1.100!4567
	client = ixp_mount(path);
	if (!client)
		return 1;
	return 0;

}

int
ubqt_data_loop(char *path)
{

	//read event, update based on string result from reads
	//ubqt_data_update(name, path)
	//ubqt_draw_new_data_callback()
	//ubqt_data_remove(name)
	//client.fd, msize, lastfid
	ixp_unmount(client);
	return 0;

}

int
ubqt_data_write(char *name, char *buffer)
{

	//Send Twrite
	return 0;

}

char *
ubqt_data_read(char *name, char *path)
{

	//Send Tread
	//-codeblock- ignore
	return "test";

}
