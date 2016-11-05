#include <stdio.h>

/* Main functions */
void ubqt_initialize(char *title);
void ubqt_run_loop();
void ubqt_destroy();
void ubqt_update_buffer();
void ubqt_update_input();
void ubqt_handle_keypress();
void ubqt_refresh();

/* Shared functions */
int ubqt_check_input();
int ubqt_check_file(int);

void ubqt_write_buffer();
char *ubqt_read_buffer(char *, int, int);



/* TODO: Add your function definition here */
void ubqt_build_menu(char *t, char *c);
void ubqt_build_buff(char *t, char *c);
void ubqt_build_nav(char *t, char *c);

struct Ubqt_Item {
  char *item_name;
  char *item_value;
} Ubqt_Item;

/* Main structure */
struct {
  char *current_out;
  char *current_in;
  char *title;
  char *path;
  char input[1000];
  int index;
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

// I rock
