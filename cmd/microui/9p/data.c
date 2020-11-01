#define IXP_NO_P9_ //no prefix
#define IXP_P9_STRUCTS
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ixp.h>
#include "../../../src/altid.h"

#define MAX_EVENTS 16
IxpClient *client;

enum fileMap {
	FEED = 1,
	STAT = 2,
	TITL = 4,
	TABS = 8,
	SIDE = 16,
	DOCU = 32,
	FORM = 64,
	STRM = 128,
}; 

char *
unmap(int key) {
	switch (key) {
	case FEED:
		return "feed";
	case STAT:
		return "status";
	case TITL:
		return "title";
	case TABS:
		return "tabs";
	case SIDE:
		return "side";
	case DOCU:
		return "document";
	case FORM:
		return "form";
	case STRM:
		return "stream";
	}
	return "";
}

int
map(char *val) {
	if(!strcmp(val, "feed"))
		return FEED;
	else if (!strcmp(val, "stat"))
		return STAT;
	else if (!strcmp(val, "title"))
		return TITL;
	else if (!strcmp(val, "tabs"))
		return TABS;
	else if (!strcmp(val, "side"))
		return SIDE;
	else if (!strcmp(val, "document"))
		return DOCU;
	else if (!strcmp(val, "form"))
		return FORM;
	else if (!strcmp(val, "stream"))
		return STRM;
	return 0;
}

int
dir_to_mask() {
	IxpMsg m;
	IxpCFid *fid;
	Stat *stat;
	char *buf;
	int i, count, mask, nstat;
	count = nstat = mask = 0;
	int mstat = 16;
	fid = ixp_open(client, "./", P9_OREAD);
	if (!fid) {
		return 0;
	}
	stat = (Stat*)malloc(sizeof(*stat) * mstat);
	buf = (char*)malloc(fid->iounit);
	while((count = ixp_read(fid, buf, fid->iounit)) > 0) {
		m = ixp_message(buf, count, MsgUnpack);
		while(m.pos < m.end) {
			if(nstat == mstat) {
				// If buffer is too small, allocate double
				mstat <<= 1;
				stat = ixp_erealloc(stat, sizeof(*stat) * mstat);
			}
			ixp_pstat(&m, &stat[nstat++]);
		}
	}
	for(i = 0; i < nstat; i++) {
		mask |= map(stat[i].name);
		ixp_freestat(&stat[i]);
	}
	free(stat);
	free(buf);
	return mask;
}

void
check_data(int bitmask) {
	char *file;
	IxpStat *stat;
	for (int i = 1; i < 256; i <<= 1 ) {
		if (!(bitmask & i)) continue;
		file = unmap(bitmask & i);
		stat = ixp_stat(client, file);
		if (stat) {
			char *buf;
			buf = altid_data_read(file, "");
			altid_data_update(file, buf);
			free(buf);
		} else {
			altid_data_remove(file);
			ixp_freestat(stat);
		}
	}
}

int
altid_data_init(char *path) {
	ixp_pthread_init();
	client = ixp_mount(path);
	if (!client)
		return 1;
	int mask = dir_to_mask();
	check_data(mask);
	return 0;
}

int
altid_data_loop(char *unused) {
	(void)unused;
	IxpCFid *eid;
	char *event;
	int initmask = 0;
	eid = ixp_open(client, "event", P9_OREAD);
	if (eid == 0) {
		// TODO: Log error
		return 1;
	}
	event = (char*)malloc(eid->iounit);
	while((ixp_read(eid, event, eid->iounit)) > 0) {		
		printf("%s\n", event);
		int mask = dir_to_mask();
		// may need to realloc
		check_data((initmask & mask) | map(event));
		altid_draw_new_data_callback();
	}
	free(event);
	ixp_close(eid);
	ixp_unmount(client);
	return 0;
}

int
altid_data_write(char *name, char *buffer) {
	IxpCFid *fid;
	// Verify we have a usable fd
	long len;
	fid = ixp_open(client, name, P9_OWRITE|P9_OTRUNC);
	if (fid == 0) {
		return 1;
	}
	len=strlen(buffer);
	if (ixp_pwrite(fid, buffer, len, 0) < len) {
		printf("Error reading %s\n", ixp_errbuf());
	}
	ixp_close(fid);
	return 0;
}

char *
altid_data_read(char *name, char *unused) {
	(void) unused;
	IxpCFid *fid;
	char *buf;
	int count;
	fid = ixp_open(client, name, P9_OREAD);
	buf = (char*)malloc(fid->iounit);
	while((count = ixp_read(fid, buf, fid->iounit)) > 0);
	ixp_close(fid);
	return buf;
}

/* TODO: 
void
altid_clear_notification() {
	// TODO: Does this return error?
	ixp_remove(client, const char "notify");
}
*/
