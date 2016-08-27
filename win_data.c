/* functions to fill win struct */
#include <stdlib.h>
#include <stdio.h>
#include "ubqt.h"

void ubqt_build_menu(char *t, char *c) {
  /* Grow char array pointer */
  ubqt_win.menu_size++;

  /* Reallocate if necessary for member size */

}

void ubqt_build_buff(char *t, char *c) {
  ubqt_win.buff_size++;
}

void ubqt_build_nav(char *t, char *c) {
  ubqt_win.nav_size++;
}
