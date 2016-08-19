#include <menu.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "../ubqt.h"

/* Set up many windows, compose together in a decent UI */

void
ubqt_initialize(char *title)
{
  win.title = title;
  initscr();
  raw();
  keypad(stdscr, TRUE);
}

void
ubqt_run_loop()
{
  /* This should simply be listening for keystrokes, and firing commands */ 
  char input[80] = "";
    
  int row, col;
  getmaxyx(stdscr, row, col);
  while(strcmp(input, ":quit")) {
    clear();
    mvprintw(row - 1, 0, "> %s", input); 
    getstr(input);
    refresh();
  }
}

void
ubqt_destroy()
{
  /* This should free any structures */
  endwin();
}

void
ubqt_navigation(char *t) {
  // This will be like buffers.pl, tabs, etc. If there's multiple files to iterate over, we will check them.
}

void ubqt_buffer_in(char *t) { 
  /* Build paths into path array*/
  win.buffer_in = t;
}

void ubqt_menu(char *t) { 
  /* Build menu items into menu array */
}
