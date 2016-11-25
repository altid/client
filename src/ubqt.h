#include <stdio.h>
#include <pthread.h>
extern pthread_mutex_t mutex;

/* Main functions */
int ubqt_connection_init(char *);
int ubqt_data_init(char *);
void ubqt_notify_loop(char *);
void ubqt_data_update(char *, char*);
void ubqt_data_remove(char *);
void ubqt__input_update(int);

/* Backend specific */
char * ubqt_draw_error(int); 	/* Wrap any fatal errors with useful messages */
int ubqt_draw_init(char *);
int ubqt_draw_loop();
int ubqt_draw_destroy();
void ubqt_update_buffer();

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



/* Input sent only on proper enter
 * char *ubqt_input;
 * When on a mutable buffer that isn't input we send chunk-by-chunk to the server TODO: Define best chunk size
 * char *ubqt_text_chunk;
 */
