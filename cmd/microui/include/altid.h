#include <pthread.h>
#include <stdbool.h>

enum {
    UBQT_SUCCESS= 0,
    UBQT_FAILURE= 1,

    MAINBUF= 0,
    STATBUF= 1,
    TITLBUF= 2,
    TABSBUF= 3,

    SCROLLSTEP = -30,
};

typedef struct Buffer Buffer;
struct Buffer {
    char name[256];
    char server[256];
    Buffer *next;
};

typedef struct Service Service;
struct Service {
    char name[256];
    char addr[256];
    bool isfirst;
    Service *next;
};


pthread_mutex_t lock; 
Buffer *list_tabs(void);
void* initmdns(void*);
void swap(Buffer*);
void freebuff(Buffer*);
void scanmdns(Service**);
void freeservice(Service*);
void send_input(const char *);
void write_file(int, const char *);
void draw_loop(Service*);
void connect_svc(Service*);
