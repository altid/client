#include "../ubiquitous.h"
#include <menu.h>
#include <ncurses.h>

/* Connect primitives to json definition */

void
initialize(char *title)
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
navigate_back()
{
}

void
navigate_forward()
{
}

void
destroy()
{
  endwin();
}
