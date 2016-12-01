#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>


/********************************************************/
/*                                                      */
/* These files represent the full state of the program  */
/* at any given time. Your UI should draw them whenever */
/* they are not-null; using this mutex to lock them.    */
/* isblock should be set for parser to know if we're in */
/* a code block or not                                  */
/*                                                      */
/********************************************************/
extern pthread_mutex_t mutex;

struct Ubqt_win {
	char *main;
	char *input;
	char *title;
	char *tabs;
	char *status;
	char *sidebar;
	char *slideout;
} ubqt_win;

enum {
	UBQT_SUCCESS = 0,
	UBQT_FAILURE = 1,
};

/**util.h************************************************/
/*                                                      */
/* These functions are provided                         */
/* watch this line for changes                          */
/* Non-void return UBQT_SUCCESS or UBQT_FAILURE         */
/*                                                      */
/* ubqt_substr(char startindex endcount)                */
/*    	squash string, from 'start' to 'end' chars      */
/*                                                      */
/* ubqt_insert(allocated string other string offset)    */
/*     insert char array at point n in string           */
/*                                                      */
/* ubqt_input_update(file, buffer                       */
/*     push buffer to named file                        */
/*                                                      */
/* ubqt_data_remove(ubqt_win.arg)                       */
/*     sets arg to null                                 */
/*                                                      */
/* ubqt_data_update(data, path)                         */
/*     threadsafe setter for ubqt_win.path              */
/*                                                      */
/* ubqt_data_destroy()                                  */
/*     clean up all data structures                     */
/*                                                      */
/********************************************************/
int   ubqt_substr(char *, unsigned, unsigned);
char *ubqt_insert(char *, const char *, unsigned);
int   ubqt_input_update(char *, char *);
void  ubqt_data_update(char *, char *);
void  ubqt_data_remove(char *);
void  ubqt_data_destroy();



/**Inotify, socket, 9p***********************************/
/*                                                      */
/* ubqt_data_read(name, path)                           */
/*     reads data from named file in path               */
/*     calls ubqt_markup_code ubqt_markup_line as needed*/
/*     returns pointer to stack-allocated data          */
/*     (data freed in ubqt_data_update, don't free it)  */
/*                                                      */
/* ubqt_data_init(path)                                 */
/*     given a path, load up ubqt_win struct            */
/*                                                      */
/* ubqt_data_loop(path)                                 */
/*     handle all events (seperate thread) firing off   */
/*     ubqt_data_update and ubqt_draw_new_data_callback */
/*     on new data                                      */
/*                                                      */
/********************************************************/
char *ubqt_data_read(char *, char *);
int   ubqt_data_init(char *);
int   ubqt_data_loop(char *);


/**cairo, nuklear, ncurses*******************************/
/*                                                      */
/* ubqt_draw_init(path)                                 */
/*     initialize graphics backend, set title from path */
/*                                                      */
/* ubqt_draw_loop                                       */
/*     handle all draw events (main loop)               */
/*                                                      */
/* ubqt_draw_destroy                                    */
/*     clean up graphics backend on shutdown            */
/*                                                      */
/* ubqt_draw_new_data_callback                          */
/*     when data is written in data loop, this function */
/*     will be called so you are able to redraw with    */
/*     the new data.                                    */
/*                                                      */
/********************************************************/
int ubqt_draw_init(char *);
int ubqt_draw_loop();
int ubqt_draw_destroy();
int ubqt_draw_new_data_callback();


/**vi, leanback******************************************/
/*                                                      */
/* ubqt_input_init                                      */
/*     set up any structures needed                     */
/*                                                      */
/* ubqt_input_loop                                      */
/*     listen for callbacks from draw loop update data  */
/*     sets with cursor position                        */
/*     and any other changes (highlighting)             */
/*                                                      */
/* ubqt_input_destroy                                   */
/*     destroy any allocated structures                 */
/*                                                      */
/********************************************************/
int ubqt_input_init();
int ubqt_input_loop();
int ubqt_input_destroy();


/**Convert markup for draw backend***********************/
/*                                                      */
/* ubqt_markup_line(line)                               */
/*     parse line in to backend specific markup         */
/*                                                      */
/* ubqt_markup_code(line)                               */
/*     mark up blockquotes, appearing as                */
/*     ```                                              */
/*     some code                                        */
/*     ```                                              */
/*     server side; this function is called on only the */
/*     "some code" line.                                */
/*                                                      */
/* Tag struct set when we have open tag available       */
/* img_main_index holds a stack of our images - images  */
/* only supported in main element, so this holds the    */
/* offsets in text where they reside                    */
/*                                                      */ 
/********************************************************/
char *ubqt_markup_line(char *);
char *ubqt_markup_code(char *);

struct Tag {
	unsigned *img_main_index;
	unsigned in_list;
	bool strong_em;
	bool uu_line;
	bool u_line;
	bool strike;
	bool code;
	bool em;
} tag_open;

 /* When on a mutable buffer that isn't input we send chunk-by-chunk to the server TODO: Define best chunk size char *ubqt_text_chunk; */
