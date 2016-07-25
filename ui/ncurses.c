#include "../ubiquitous.h"
#include <ncurses.h>

/* Connect primitives to json definition */

void
initialize() {
  initscr();
  raw();
}

void
set_title(char *title) {
  (void) title;
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
