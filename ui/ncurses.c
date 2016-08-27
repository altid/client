#include <menu.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>

#include "../ubqt.h"

//TODO: Use noecho() and handle input character by character

/* Set up many windows, compose together in a decent UI */

void
ubqt_initialize(char *title)
{
  ubqt_win.title = title;
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

