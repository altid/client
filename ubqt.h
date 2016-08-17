#include <stdio.h>

/* Main functions */
void ubqt_initialize(char *);
void ubqt_destroy();
void ubqt_run_loop();

/* Optional functions */
void ubqt_menu_save(char *t);
void ubqt_menu_quit(char *t);
void ubqt_buffer_in(char *t);
void ubqt_navigation(char *t);

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
} UBQT_Func;

enum mitem {
  SAVE,
  QUIT,
};

static UBQT_Func ubqt_func[] = {
  /* Key  token function  */
  { "menu",       "save",    ubqt_menu_save }, 
  { "menu",       "quit",    ubqt_menu_quit },
  { "buffers",    "main",    ubqt_buffer_in }, 
  { "navigation", "next",    ubqt_navigation},
};
