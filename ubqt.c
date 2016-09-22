#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "ubqt.h"

int
main(int argc, char* argv[])
{

  if (argc != 2) {
    fprintf(stderr, "usage: %s %s.toml\n", argv[0], argv[0]);
    return 2;
  }

  ubqt_win.menu_size = 0;
  ubqt_win.buff_size = 0;
  ubqt_win.navi_size = 0;
  ubqt_win.current_out = "/home/halfwit/irc/irc.freenode.net/#teamcool/out";
  ubqt_win.current_in = "/home/halfwit/irc/irc.freenode.net/#teamcool/in";
  ubqt_win.path = "/home/halfwit/irc/irc.freenode.net";

  /* Set up windows and handle mounting if necessary */
  ubqt_initialize(argv[0]);
  
  /* Interactive loop */
  ubqt_run_loop();

  ubqt_destroy();

  return 0;
}
