#include <menu.h>
#include <ncurses.h>
#include <string.h>

#include "../ubiquitous.h"

void
ub_initialize(char *title)
{
  (void) title;
  initscr();
  raw();
  keypad(stdscr, TRUE);
 }

void
ub_run_loop()
{
  char input[80] = "tmp";
  int row, col;
  while(strcmp(input, "/quit")) {
    getstr(input);
    getmaxyx(stdscr, row, col);
    refresh();
    mvprintw(row - 1, 0, "> %s", input);
  }
  //refresh();
}

void
ub_destroy()
{
  endwin();
}

void
ub_menu() {
 
}

void
ub_input() {

}

void
ub_buffer() {

}

void
ub_title() {
  // This will be off by default for me, but let's set it anyways.
  int row, col;
  getmaxyx(stdscr, row, col);
  mvprintw(0, 0, "This is a title");
}

void
ub_navigation_next(char *t) {
  // This will be like buffers.pl, tabs, etc. If there's multiple files to iterate over, we will check them.
}

void ub_buffer_main(char *t) { 
}
void ub_menu_quit(char *t) { 
}
void ub_menu_save(char *t) { 
  printf("Works! %s\n", t);
} 
