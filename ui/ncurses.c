#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/select.h>
#include <ncurses.h>
#include "../ubqt.h"
#include "../shared/vi.h"

char buf_in[1024];
int row, col; 
/* Set up many windows, compose together in a decent UI */

void
ubqt_initialize(char *title)
{
  ubqt_win.title = title;
  if (!initscr()) {
    fprintf(stderr, "Error calling initscr()\n");
    exit(EXIT_FAILURE);
  }
  keypad(stdscr, TRUE);
  timeout(0);
  noecho();
  scrollok(stdscr, TRUE);
  //start_color();
  refresh(); 
  //ubqt_vi_init();
}

unsigned
ubqt_handle_keypress(int ch, unsigned index) {

  //ubqt_win.input.buf && ubqt_win.input.index
  switch(ch) {
  case '\n':
    /* Null terminate string, send to FIFO */
    buf_in[index + 1] = 0;
    ubqt_write_buffer(buf_in);

    /* Reset string */
    buf_in[0] = 0;
    index = 0;
    break;
  case KEY_BACKSPACE:
    if(index >= strlen(ubqt_vi_mode_get()) - 1) {
      buf_in[--index] = 0;
    }
    break;
  case KEY_RESIZE:
    endwin();
    getmaxyx(stdscr, row, col);
    break;
  default: 
    buf_in[index] = ch;
    buf_in[++index] = 0;
    break;
  }
  return (index > 0) ? index : 0;
}

/* on data write, resize */
void
ubqt_update_buffer() {
  getmaxyx(stdscr, row, col);
  mvprintw(0, 0, "%s", ubqt_read_buffer());
  /* refresh what is needed to be refreshed with panels lib */

}

void
ubqt_update_input(char *buf) {

  if(strcmp(buf, ":quit")) {
    ubqt_destroy();
  }

  getmaxyx(stdscr, row, col);
  mvprintw(row - 1, 0, "%s %s", ubqt_vi_mode_get(), buf);
  refresh();
}

void
ubqt_destroy()
{
  /* This should free any structures */
  echo();
  endwin();
  exit(0);
}

