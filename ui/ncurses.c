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
ubqt_ncurses_handle_keypress(int ch, unsigned index, char *buf_in) {

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

void
ubqt_run_loop()
{
 
  /* Sane default for now, will change to dynamic eventually */
  char buf_in[1024];
  unsigned index = 0;
  /* last time mtime */
  getmaxyx(stdscr, row, col);
  
  /* Temporary test to exit cleanly */
  while(strcmp(buf_in, ":quit")) {
    
    /* Use stat here to test if buffer has new mtime*/
    //fdstat as well. Will have to just pass buff around
    char *data = ubqt_read_buffer();
    /* Then if it does, grab chunk twice the size of row * col */
    printw("%s", data);
    mvprintw(row - 1, 0, "%s %s", ubqt_vi_mode_get(), buf_in); 
    refresh();
    
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 700000;

    int status = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
   
    /* If we have any input */
    if ( status == -1 && errno != EINTR ) { 
      ubqt_destroy();
    } else if (FD_ISSET(STDIN_FILENO, &fds)) {
      index = ubqt_ncurses_handle_keypress(getch(), index, &buf_in[0]);
    }
    
    free(data);
  }
}

void
ubqt_destroy()
{
  /* This should free any structures */
  echo();
  endwin();
}

