// Main drawing functions
void ub_initialize(char *);
void ub_destroy();
void ub_run_loop();
void ub_menu_save(char *t);
void ub_menu_quit(char *t);
void ub_buffer_main(char *t);
void ub_navigation_next(char *t);

typedef struct {
  const char *t;
  const char *k;
  void (*func)(char *v);
} Conf;

static Conf ub_conf[] = {
  /* Key  token function  */
  { "menu",       "save",    ub_menu_save }, 
  { "menu",       "quit",    ub_menu_quit },
  { "buffers",    "main_in", ub_buffer_main }, 
  { "navigation", "next",    ub_navigation_next },

};
