#include <pthread.h>

enum {
    UBQT_SUCCESS= 0,
    UBQT_FAILURE= 1,

    MAINBUF= 0,
    STATBUF= 1,
    TITLBUF= 2,
    TABSBUF= 3,

    SCROLLSTEP = -30,
};

typedef struct Service Service;
struct Service {
    char name[256];
    char addr[256];
    uint16_t port;
    Service *next;
};


pthread_mutex_t lock; 

Service* scanmdns(void);
void freeservice(Service*);
void send_input(const char *);
void write_file(int, const char *);
void draw_loop(void);
