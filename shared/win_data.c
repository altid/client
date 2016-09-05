/* functions to fill win struct */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../ubqt.h"

/* ubqt_win.menu --> allocate more if needed */

/* Eventually get file chunks instead of one-shots */
char *ubqt_read_buffer(int buf_size) {
  char *buf = NULL;
  FILE *fp;
  int str_sz;
  fp = fopen(ubqt_win.current, "r");

  if(fp)
  {
    fseek(fp, 0, SEEK_END);
    str_sz = ftell(fp);
    rewind(fp);

    buf = (char*) malloc(sizeof(char) * (str_sz + 1));

    fread(buf, sizeof(char), str_sz, fp);
    buf[str_sz] = '\0';
    fclose(fp);
  } 
  return strdup(buf);
}

void ubqt_write_buffer(char *input) {
  FILE *fp;
  fp = fopen(ubqt_win.input, "w");
  fprintf(fp, "%s\n", input);
  fclose(fp);
}

void ubqt_build_menu(char *t, char *c) {
  
  //ubqt_win.menu[ubqt_win.menu_size].item_name = t;
  //ubqt_win.menu[ubqt_win.menu_size].item_value = c;
  //ubqt_win.menu_size++;
}

void ubqt_build_buff(char *t, char *c) {
  //ubqt_win.buff[ubqt_win.buff_size].item_name = t;
  //ubqt_win.buff[ubqt_win.buff_size].item_value = c;
  //ubqt_win.buff_size++;
}

void ubqt_build_nav(char *t, char *c) {
  //ubqt_win.navi[ubqt_win.navi_size].item_name = t;
  //ubqt_win.navi[ubqt_win.navi_size].item_value = c;
  //ubqt_win.navi_size++;
}
