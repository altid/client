enum {
    UBQT_SUCCESS= 0,
    UBQT_FAILURE= 1,
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

int scanmdns(DNSData*);