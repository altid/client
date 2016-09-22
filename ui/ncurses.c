#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/select.h>
#include <ncurses.h>
#include <panel.h>
#include "../src/ubqt.h"
#include "../shared/vi.h"

int row, col; 
WINDOW *win[2];
/* Set up many windows, compose together in a decent UI */

void
ubqt_initialize(char *title)
{
  ubqt_win.title = title;
  ubqt_win.index = 0;
  if (!initscr()) {
    fprintf(stderr, "Error calling initscr()\n");
    exit(EXIT_FAILURE);
  }
  keypad(stdscr, TRUE);
  timeout(0);
  noecho();
  //start_color();
  //ubqt_vi_init();

  getmaxyx(stdscr, row, col);

  /* main buffer */
  win[0] = newwin(row, col, 0, 0);
  scrollok(win[0], TRUE);
  /* input */
  win[1] = newwin(1, col, row - 1, 0);

  wprintw(win[0], "%s", ubqt_read_buffer(ubqt_win.current_out, 0, 0));
  mvwprintw(win[1], 0, 0, "%s ", ubqt_vi_mode_get());
  wnoutrefresh(win[0]);
  wnoutrefresh(win[1]);
  doupdate();
  
}

void
ubqt_handle_keypress() {
  
  int ch = getch();
  switch(ch) {
  case '\n':
    /* Null terminate string, send to FIFO */
    ubqt_win.input[ubqt_win.index + 1] = 0;
    ubqt_write_buffer();

    /* Reset string */
    ubqt_win.input[0] = 0;
    ubqt_win.index = 0;
    break;

  case KEY_BACKSPACE:
    if(ubqt_win.index >= strlen(ubqt_vi_mode_get()) - 1) {
      ubqt_win.input[--ubqt_win.index] = 0;
    }
    break;

  case KEY_RESIZE:
    getmaxyx(stdscr, row, col);
    wresize(win[0], row, col);
    wresize(win[1], 1, col);
    wclear(win[0]);
    wclear(win[1]);
    ubqt_refresh();
    break;

  default: 
    ubqt_win.input[ubqt_win.index] = ch;
    ubqt_win.input[++ubqt_win.index] = 0;
    break;
  }
}

/* on data write, resize */
void
ubqt_update_buffer() {
  wprintw(win[0], "%s", ubqt_read_buffer(ubqt_win.current_out, 0, 0));
  wclrtobot(win[0]);
}

void
ubqt_update_input() {

  if(!strcmp(ubqt_win.input, ":quit")) {
    ubqt_destroy();
  }
  wclear(win[1]);
  mvwprintw(win[1], 0, 0, "%s %s", ubqt_vi_mode_get(), ubqt_win.input);
}

void
ubqt_refresh() {
  wnoutrefresh(win[0]);
  wnoutrefresh(win[1]);
  doupdate();
}

void
ubqt_destroy()
{
  /* This should free any structures */
  echo();
  delwin(win[0]);
  delwin(win[1]);
  endwin();
  exit(0);
}

