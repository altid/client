#include "../ubiquitous.h"
#include <ncurses.h>
#include <menu.h>

/* Connect primitives to json definition */

void
initialize() {
  initscr();
  raw();
  keypad(stdscr, TRUE);
  int row, col;
  getmaxyx(stdscr, row, col);
  mvprintw(row - 1, 0, "Hello world!");
}

void
set_title(char *title) {
  mvprintw(0, 0, title);
  refresh();
  getch();
}

//getch(); // wait for input

void
navigate_back() {

}

void
navigate_forward() {

}

void 
destroy() {
  endwin();
}
