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

/* scan.c */
typedef struct DNSData DNSData;

struct DNSData {
	// We need a channel of DNSEntry
};

struct DNSEntry {
    // name
    // dialstring/port
};

pthread_mutex_t lock; 

int scanmdns(DNSData*);
void send_input(const char *);
void write_file(int, const char *);
void draw_loop(void);
