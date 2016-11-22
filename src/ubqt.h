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

enum filename {
		TITLE = 0,
		INPUT,
		STATUS,
		TEXT,
		SIDEBAR,
		SLIDEOUT,
		STREAM,
		CTL,
		CMPL,
};

/* Input sent only on proper enter
 * char *ubqt_input;
 * When on a mutable buffer that isn't input we send chunk-by-chunk to the server TODO: Define best chunk size
 * char *ubqt_text_chunk;
 */
