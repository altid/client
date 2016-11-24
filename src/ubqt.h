#include <stdio.h>

/* Main functions */
void ubqt_connection_init(const char *);
void ubqt_window_init();
void ubqt_file_loop();
void ubqt_main_loop();
void ubqt_destroy();
int ubqt_data_init();
void ubqt_data_update(char *);
void ubqt_data_remove(char *);

/* Shared functions */
void ubqt_update_buffer();
void ubqt_update_input(int);
void ubqt_redraw();
char * ubqt_file_read(char *);

void ubqt_update_element();

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
