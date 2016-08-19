#include <stdio.h>

/* Main functions */
void ubqt_initialize(char *);
void ubqt_run_loop();
void ubqt_destroy();

/* Optional functions */
void ubqt_menu_save(char *t);
void ubqt_menu_quit(char *t);
void ubqt_buffer_in(char *t);
void ubqt_navigation(char *t);

struct {
  char *buffer_in;
  char *buffer_out;
  char *title;
  char *input;
  char **navi;
  char *menu[] ;
} win;

struct Ubqt_func {
  const char *t;
  const char *k;
  void (*func)(char *v);
};

static struct Ubqt_func ubqt_func[] = {
  /* Key  token function  */
  { "menu",       "save",    ubqt_menu }, 
  { "menu",       "quit",    ubqt_menu },
  { "buffers",    "main",    ubqt_buffer_in }, 
  { "navigation", "next",    ubqt_navigation},
};
