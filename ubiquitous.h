#include <stdio.h>

// Main drawing functions
void ub_initialize(char *);
void ub_destroy();
void ub_run_loop();
void ub_menu_save(char *t);
void ub_menu_quit(char *t);
void ub_buffer_in(char *t);
void ub_navigation(char *t);

struct {
  FILE *buffer_in;
  FILE *buffer_out;
  char *title;
  char *input;
  char **navi;
  char *menu[] ;
} win;

typedef struct {
  const char *t;
  const char *k;
  void (*func)(char *v);
} UB_Func;

enum mitem {
  SAVE,
  QUIT,
};

static UB_Func ub_func[] = {
  /* Key  token function  */
  { "menu",       "save",    ub_menu_save }, 
  { "menu",       "quit",    ub_menu_quit },
  { "buffers",    "main",    ub_buffer_in }, 
  { "navigation", "next",    ub_navigation},

};
