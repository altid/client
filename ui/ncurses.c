#include "../ubiquitous.h"
#include <menu.h>
#include <ncurses.h>

/* Connect primitives to json definition */

void
ub_initialize(char *title)
{
  initscr();
  raw();
  keypad(stdscr, TRUE);
  int row, col;
  getmaxyx(stdscr, row, col);
  mvprintw(row - 1, 0, "Hello world!");
  refresh();
  getch();
}

void
ub_navigate_back()
{
}

void
ub_navigate_forward()
{
}

void
ub_destroy()
{
  endwin();
}
