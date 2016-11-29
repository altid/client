#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>


/********************************************************/
/*                                                      */
/* These files represent the full state of the program  */
/* at any given time. Your UI should draw them whenever */
/* they are not-null; using this mutex to lock them.    */
/*                                                      */
/********************************************************/
extern pthread_mutex_t mutex;
struct Ubqt_win {
	char *main;
	char *input;
	char *title;
	char *tabbar;
	char *status;
	char *sidebar;
	char *slideout;
} ubqt_win;


/**util.h************************************************/
/*                                                      */
/* These functions are provided                         */
/* watch this line for changes                          */
/*                                                      */
/* ubqt_join                                            */
/*     appends a to b, returning result                 */
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
/********************************************************/
char *ubqt_join(char *, char *);
void  ubqt_input_update(char *, char*);
void  ubqt_data_remove(char *);
void  ubqt_data_update(char *, char*);



/**Inotify, socket, 9p***********************************/
/*                                                      */
/* ubqt_data_init(path)                                 */
/*     given a path, load up ubqt_win struct            */
/*                                                      */
/* ubqt_data_loop(path)                                 */
/*     handle all events (seperate thread) firing off   */
/*     ubqt_data_update and ubqt_draw_new_data_callback */
/*     on new data                                      */
/*                                                      */
/* ubqt_data_read(name, path)                           */
/*     reads data from named file in path               */
/*                                                      */
/********************************************************/
int   ubqt_data_init(char *);
void  ubqt_data_loop(char *);
char *ubqt_data_read(char *, char *path);


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
int  ubqt_draw_init(char *);
int  ubqt_draw_loop();
int  ubqt_draw_destroy();
void ubqt_draw_new_data_callback();


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
/*     return line with backend specific markup         */
/*                                                      */
/* ubqt_markup_code(line)                               */
/*     mark up blockquotes, appearing as                */
/*     ```                                              */
/*     some code                                        */
/*     ```                                              */
/*     server side; this function is called on only the */
/*     "some code" line.                                */
/*                                                      */
/********************************************************/
char *ubqt_markup_line(char *);
char *ubqt_markup_code(char *);

 /* When on a mutable buffer that isn't input we send chunk-by-chunk to the server TODO: Define best chunk size char *ubqt_text_chunk; */
