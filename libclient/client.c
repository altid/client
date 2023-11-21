#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"

static struct { const char *file; int idx; } event_table[] = {
    { "title", ALT_EVENT_TITLE },
    { "status", ALT_EVENT_STATUS },
    { "aside", ALT_EVENT_ASIDE },
    { "tabs", ALT_EVENT_TABS },
    { "main", ALT_EVENT_MAIN },
    { "notification", ALT_EVENT_NOTIFY },
    { "error", ALT_EVENT_ERR },
    { NULL }
};

int svc_oper(alt_client *, const char *, void *, int(*)(const char *, int, int));

int alt_msg_send(alt_client *cl, const char *msg) { return svc_oper(cl, "input", msg, cl->write); }
int alt_cmd_send(alt_client *cl, alt_cmd *cmd) { return svc_oper(cl, "ctl", cmd->data, cl->write); }

int
alt_cmds_build(alt_client *cl)
{
    char *buf;
    int d;
    d = svc_oper(cl, "ctl", &buf, cl->read);
    if (d != 0) {
        return d;
    }
    /* Build into a linked list of commands in cl */
}

/* Run the given operation, read/write */
int svc_oper(alt_client *cl, const char *target, void *data, int (*oper)(const char *, int, int)) {
    int fd;
    if(cl->open(target, &fd) < 0){
        return ALT_ERR_OPEN;
    }
    if(oper(data, sizeof(data), fd) < 0){
        return ALT_ERR_ACTION;
    }
    if(cl->close(fd) < 0) {
        return ALT_ERR_CLOSE;
    }
}

int
alt_next(alt_client *cl, alt_event *event)
{
    char *data;
    static int efd;

    if(efd <= 0 && cl->open("events", &efd) < 0){
        strcpy(event->data, "Unable to open events file");
        goto ERR;
    }
    if(cl->read(&data, sizeof(data), &efd) < 0){
        strcpy(event->data, "Unable to read events file");
        goto ERR;
    }
    /* Read in from efd, break into lines, build events array */
    return event;
ERR:
    event->type = ALT_EVENT_ERR;
    event->size = strlen(event->data);
    event->next = NULL;
    return event;
}


alt_event *
convS2E(const char *msg, int type)
{
    alt_event *event;
    event = malloc(sizeof *event);
    event->type = type;
    strcpy(event->data, msg);
    event->size = sizeof(event->data);
    event->next = NULL;
    return event;
}