#include <stdio.h>

/* Main functions */
void ubqt_initialize(char *);
void ubqt_run_loop();
void ubqt_destroy();

/* TODO: Add your function definition here */
void ubqt_build_menu(char *t, char *c);
void ubqt_build_buff(char *t, char *c);
void ubqt_build_nav(char *t, char *c);



/* Main structure */
struct {
  char *curr_buff;
  char *title;
  char *input;
  char **menu;
  size_t menu_size;
  size_t menu_max;
  char **buff;
  size_t buff_size;
  size_t buff_max;
  char **nav;
  size_t nav_size;
  size_t nav_max;
} ubqt_win;

struct Ubqt_func {
  const char *t;
  const char *k;
  void (*func)(char *k, char *v);
};

/* TODO: Add your struct entries here */
static struct Ubqt_func ubqt_func[] = {
 /* Item          Action     Function */
  { "menu",       "save",    ubqt_build_menu }, 
  { "menu",       "quit",    ubqt_build_menu },
  { "buffers",    "main",    ubqt_build_buff }, 
  { "navigation", "next",    ubqt_build_nav  },
};
