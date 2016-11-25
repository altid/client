#include <stdio.h>
#include <pthread.h>
extern pthread_mutex_t mutex;

/* Main functions */
int ubqt_connection_init(char *);
int ubqt_data_init(char *);
void ubqt_file_loop(char *);
void ubqt_data_update(char *, char*);
void ubqt_data_remove(char *);
char * ubqt_file_read(char *, char *);
void ubqt_update_input(int);

/* Backend specific */
char * ubqt_main_error(int); 	/* Wrap any fatal errors with useful messages */
int ubqt_window_init(char *);
int ubqt_main_loop();
int ubqt_destroy();
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
