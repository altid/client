/* altid/client.h */


#ifndef ALTID_CLIENT_H
#define ALTID_CLIENT_H

#include <limits.h>
#include <stdio.h>

#include "atomic_ops.h"

enum {
    ALT_ERR_OPEN = -1,
    ALT_ERR_ACTION = -2,
    ALT_ERR_CLOSE = -3
};

enum {
    ALT_CMD_BUFF = 1,
    ALT_CMD_SVC,
    ALT_CMD_QUIT,
    ALT_CMD_ERR,

    /* No services exist with more than this yet */
    ALT_CMD_MAX = 24
};

enum {
    ALT_EVENT_TITLE = 1,
    ALT_EVENT_STATUS,
    ALT_EVENT_ASIDE,
    ALT_EVENT_TABS,
    ALT_EVENT_MAIN,
    ALT_EVENT_NOTIFY,
    ALT_EVENT_ERR,
    ALT_EVENT_NULL
};

typedef intptr_t val_t;

// boolean CAS_PTR for ease of coding.
#define CAS_PTR_BOOL(addr, old, new) (old == CAS_PTR(addr, old, new))

typedef struct node {
    val_t val;
    struct node *next;
} node_t;

typedef struct lqueue {
    node_t *head; // INT_MIN
    node_t *tail; // INT_MAX
} lqueue_t;

typedef struct cmd {
    int type;
    int size;
    const char *data;
} cmd_t;

// Memory pool, used internally in the queue
node_t **mem; // Memory blocks
uint32_t memptr; // Current cell
#define MEM_BLOCK_SIZE 20000
#define MEM_BLOCK_CNT 100

lqueue_t* queue_new();
val_t queue_pop(lqueue_t *q);
void queue_push(lqueue_t *q, val_t val);
void queue_free(lqueue_t *q);

// Conversion functions, not reentrant.
int convS2E(char *);
cmd_t *convS2C(char *);
char *convC2S(cmd_t *);

#endif
