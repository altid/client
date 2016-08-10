#include <menu.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "../ubiquitous.h"

void
ub_initialize(char *title)
{
  win.title = title;
  initscr();
  raw();
  keypad(stdscr, TRUE);
 }

void
ub_run_loop()
{
  char input[80] = "";
  int row, col;
  getmaxyx(stdscr, row, col);
  mvprintw(row - 1, 0, "> "); 
  while(1) {
    getstr(input);
    clear();
    mvprintw(row - 1, 0, "> %s", input); 
    if(!strcmp(input, "/quit")) {
      break;
    }
  }
}

void
ub_destroy()
{
  endwin();
}

void
ub_navigation(char *t) {
  // This will be like buffers.pl, tabs, etc. If there's multiple files to iterate over, we will check them.
}

void ub_buffer_in(char *t) { 
  //FILE *f;
}
void ub_menu_quit(char *t) { 
  win.menu[QUIT] = t;
}
void ub_menu_save(char *t) { 
  win.menu[SAVE] = t;
} 
