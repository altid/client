/* altid/client.h */

#ifndef ALTID_CLIENT_H
#define ALTID_CLIENT_H

typedef struct alt_client alt_client;

enum {
    ALT_ERR_OPEN,
    ALT_ERR_ACTION,
    ALT_ERR_CLOSE
};

enum {
    ALT_CMD_BUFF = 1,
    ALT_CMD_ERR
};

enum {
    ALT_EVENT_SEND = 1,
    ALT_EVENT_TITLE,
    ALT_EVENT_STATUS,
    ALT_EVENT_ASIDE,
    ALT_EVENT_TABS,
    ALT_EVENT_MAIN,
    ALT_EVENT_NOTIFY,
    ALT_EVENT_NULL,
    ALT_EVENT_ERR
};

typedef struct {
    int type;
    int size;
    const char *data;
    alt_cmd *next;
} alt_cmd;

typedef struct {
    int type;
    int size;
    const char *data;
    alt_event *next;
} alt_event;

struct alt_client {
    alt_cmd    *cmds;
    int (*read)(void *data, unsigned int msize, int fd);
    int (*write)(void *data, unsigned int msize, int fd);
    int (*seek)(int whence, unsigned int offset, int fd);
    int (*rm)(const char *path, int fd);
    int (*open)(const char *path, int *fd);
    int (*close)(int fd);
};

int alt_msg_send(alt_client *, const char *);
int alt_cmd_send(alt_client *, alt_cmd *);
int alt_cmds_build(alt_client *);
int alt_next(alt_client *, alt_event *);
void alt_event_free(alt_event);
#endif
