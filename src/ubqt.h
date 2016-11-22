#include <stdio.h>

/* Main functions */
void ubqt_connection_init(const char *);
void ubqt_window_init();
void ubqt_run_loop();
void ubqt_destroy();
void ubqt_update_data();
void ubqt_remove_data();

/* Shared functions */
void ubqt_update_buffer();
void ubqt_update_input();
void ubqt_redraw();

const char *path;

/* We need an internal representation of text elements */

typedef struct {
		char *title;
		char *input;
		char *status;
		char *text;
		char *sidebar;
		char *slideout;
} Win;
