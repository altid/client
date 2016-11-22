#include <stdio.h>

/* Main functions */
void ubqt_connection_init(const char *);
void ubqt_window_init();
void ubqt_run_loop();
void ubqt_destroy();
void ubqt_update_data();

/* Shared functions */
int ubqt_check_input();
int ubqt_check_file(int);
void ubqt_update_buffer();
void ubqt_handle_input();
void ubqt_refresh();

void ubqt_write_buffer();
char *ubqt_read_buffer(char *, int, int);

const char *path;
