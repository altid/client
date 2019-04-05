#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>


/* altid_win
These files represent the full state of the program
at any given time. Your UI should draw them whenever
they are not-null; using this mutex to lock them.
isblock should be set for parser to know if we're in
a code block or not
*/
	
extern pthread_mutex_t mutex;

struct Altid_win {
	char *main;
	char *input;
	char *title;
	char *tabs;
	char *status;
	char *sidebar;
	char *slideout;
	char *path;
} altid_win;

enum {
	UBQT_SUCCESS = 0,
	UBQT_FAILURE = 1,
};

/*  util.h
These functions are provided

altid_next(string, char, startingpoint)
	find next occurance of char or return 0

altid_substr(string startindex endcount)
	squash string, from 'start' to 'end' chars

altid_insert(allocated string other string offset)
	insert char array at point n in string
	returns change in size to string

altid_replace|_ch(string, token, index, range)
	replace area at index, range with token
	returns change in size to string

altid_input_update(file, buffer
	push buffer to named file

altid_data_remove(altid_win.arg)
	sets arg to null

altid_data_update(data, path)
	threadsafe setter for altid_win.path

altid_data_destroy()
	clean up all data structures
*/

unsigned altid_next(char *, const char, unsigned);
int altid_substr(char *, unsigned, unsigned);
int altid_insert(char **, const char *, unsigned);
int altid_replace(char **, const char *, unsigned, unsigned);
int altid_replace_ch(char **, const char, unsigned, unsigned);
int altid_input_update(char *, char *);
void altid_data_update(char *, char *);
void  altid_data_remove(char *);
void  altid_data_destroy();



/* Inotify, socket, 9p
altid_data_read(name, path)
    reads data from named file in path
    calls altid_markup_code altid_markup_line as needed
    returns pointer to stack-allocated data
    (data freed in altid_data_update, don't free it)

altid_data_write(name, string)
    write data to file name 

altid_data_init(path)
    given a path, load up altid_win struct

altid_data_loop(path)
    handle all events (seperate thread) firing off
    altid_data_update and altid_draw_new_data_callback
    on new data
*/
char *altid_data_read(char *, char *);
int   altid_data_write(char *, char *);
int   altid_data_init();
int   altid_data_loop(char *);


/* cairo, nuklear, ncurses

altid_draw_init(path)
    initialize graphics backend, set title from path

altid_draw_loop
    handle all draw events (main loop)

altid_draw_destroy
    clean up graphics backend on shutdown

altid_draw_new_data_callback
    when data is written in data loop, this function
    will be called so you are able to redraw with
    the new data.
*/

int altid_draw_init(char *);
int altid_draw_loop();
int altid_draw_destroy();
int altid_draw_new_data_callback();

struct {
	char *cur; /* Name of window that holds focus */
	int index;
} cursor;

/* vi, leanback

altid_input_init
	set up any structures needed

altid_input_loop
    listen for callbacks from draw loop update data
	sets with cursor position
	and any other changes (highlighting)
altid_input_destroy
    destroy any allocated structures
*/

int altid_input_init();
int altid_input_handle(char *);
int altid_input_destroy();

// TODO: This all changes to function calls per element
// We'll use pass by value here, sending at most a line at a time 
