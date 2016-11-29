#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
extern pthread_mutex_t mutex;

/* Main functions */

/* util.c */
char *ubqt_join(char *, char *);

/* Inotify, Plan9 may be different */
int ubqt_data_init(char *);
void ubqt_data_loop(char *);
char *ubqt_data_read(char *, char *path);

void ubqt_draw_new_data_callback();


/* Provided functions */
void ubqt_data_remove(char *);
void ubqt_data_update(char *, char*);


/* Backend specific */
int ubqt_draw_init(char *);
int ubqt_draw_loop();
int ubqt_draw_destroy();

/* Fire and forget key handle from UI land */
int ubqt_input_init();
int ubqt_input_loop();
int ubqt_input_destroy();

/* Write chunk of input to file */
void ubqt_input_update();

/* Convert markup per draw backend */
char *ubqt_markup_line(char *);
char *ubqt_markup_code(char *);

/* argv[1] */

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
