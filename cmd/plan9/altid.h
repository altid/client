#include <thread.h>

/* Token type */
enum {
	ENormalText,
	EColorName,
	EColorText,
	EUrlName,
	EUrlText,
	EImagePath,
	EImageText,
	EImageLink,
	EBoldText,
	EStrikeText,
	EEmText,
	EUnderlineText,
	ErrorText,
};

/* Data struct update */
enum {
	DTabs,
	DStatus,
	DAside,
	DNavi,
	DTitle,
	DMain,
	DFeed,
};

typedef struct Tabs Tabs;
struct Tabs {
	char *name;
	int tabmode;
	Tabs *next;
};

typedef struct Data Data;
struct Data {
	char *tabs;
	char *page;
	char *input;
	char *status;
	char *aside;
	char *title;
	char *navi;
	char *root;
	int state;
};

typedef struct Message Message;
struct Message {
	char *data;
	int type;
};

typedef struct Token Token;
struct Token {
	int counter;
	int type;
	char *data;
};

typedef struct Event Event;
struct Event {
	char *msg;
	uint msize;
};

char *current;
int md_init_data(char*, Data**);
void md_input(char**);
void md_data(Data *, char *, uint);
void md_ui(struct nk_context *, Data *);

