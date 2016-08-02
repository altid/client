#include <menu.h>
#include <ncurses.h>

#include "../ubiquitous.h"

void
ub_initialize(char *title)
{
  (void) title;
  initscr();
  raw();
  keypad(stdscr, TRUE);
  int row, col;
  getmaxyx(stdscr, row, col);
  mvprintw(row - 1, 0, "Hello World!"); 
}

void
ub_draw() 
{
  refresh();
  getch();
}

void
ub_destroy()
{
  endwin();
}

void
ub_input_enter_button(const char *val) 
{ 
  (void) val;
  // draw a motherfucking enter button
}
