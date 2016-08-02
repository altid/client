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
}

void
ub_run_loop()
{
  getch();
  //refresh();
}

void
ub_destroy()
{
  endwin();
}
