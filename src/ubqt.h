#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
extern pthread_mutex_t mutex;

/* Main functions */
int ubqt_connection_init(char *);
int ubqt_data_init(char *);
void ubqt_notify_loop(char *);
void ubqt_data_update(char *, char*);
void ubqt_data_remove(char *);
void ubqt__input_update(int);
char *ubqt_join(char *, char *);

/* Backend specific */
char *ubqt_draw_error(int); 	/* Wrap any fatal errors with useful messages */
int ubqt_draw_init(char *);
int ubqt_draw_loop();
int ubqt_draw_destroy();
void ubqt_update_buffer();

/* Convert markup per draw backend */
char *ubqt_markup_line(char *);
char *ubqt_markup_code(char *);

/* argv[1] */
const char *path;

struct Ubqt_win {
	char *title;
	char *status;
	char *tabbar;
	char *input;
	char *sidebar;
	char *text;
	char *slideout;
} ubqt_win;

struct Tags {
	bool sz_xxl;
	bool sz_xl;
	bool sz_l;
	bool sz_m;
	bool sz_s;
	bool sz_xs;
	bool em;
	bool uline;
	bool strong_em;
	bool double_uline;
	bool strike;
	bool li;
	bool ul;
	bool link;
	bool image;
	bool color;
	bool code;
	bool codeblk;
} tag;

/* Input sent only on proper enter
 * char *ubqt_input;
 * When on a mutable buffer that isn't input we send chunk-by-chunk to the server TODO: Define best chunk size
 * char *ubqt_text_chunk;
 */
