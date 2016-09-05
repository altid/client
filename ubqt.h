#include <stdio.h>

/* Main functions */
void ubqt_initialize(char *);
void ubqt_run_loop();
void ubqt_destroy();
int ubqt_check_input();
void ubqt_write_buffer(char *);
char *ubqt_read_buffer();

/* TODO: Add your function definition here */
void ubqt_build_menu(char *t, char *c);
void ubqt_build_buff(char *t, char *c);
void ubqt_build_nav(char *t, char *c);

struct Ubqt_Item {
  char *item_name;
  char *item_value;
};

/* Main structure */
struct {
  char *current;
  char *input;
  char *title;
  struct Ubqt_Item *menu;
  struct Ubqt_Item *navi;
  struct Ubqt_Item *buff; 
  int menu_size;
  int navi_size;
  int buff_size;
} ubqt_win;

struct Ubqt_func {
  char *t;
  char *k;
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
