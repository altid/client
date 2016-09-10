#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "ubqt.h"
#include <toml.h>

void
ubqt_setup(struct toml_node *node, void *ctx) {
 
  //TODO: Handle in seperate source file  
  unsigned long i;
  for (i = 0; i < sizeof(ubqt_func) / sizeof(ubqt_func[0]); i++) {
    if(!strcmp(ubqt_func[i].t, (char *)ctx) && !strcmp(ubqt_func[i].k, toml_name(node))) {
      ubqt_func[i].func(ubqt_func[i].k, toml_value_as_string(node));
    } 
  }
}

void
ubqt_find_tables(struct toml_node *node, void *ctx) {
  (void) ctx;
  if(toml_type(node) == TOML_TABLE) {
    toml_node_walker fn = &ubqt_setup;

    char *name = toml_name(node);
    toml_walk(node, fn, name);
    free(name);
  }
}

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
  ubqt_win.current = "/home/halfwit/irc/irc.freenode.net/#teamcool/out";
  ubqt_win.input = "/home/halfwit/irc/irc.freenode.net/#teamcool/in";
  ubqt_win.path = "/home/halfwit/irc/";

  char *buf = NULL;
  FILE *fp;
  int str_sz;
  fp = fopen(argv[1], "r");
  
  if (fp) 
  {
    fseek(fp, 0, SEEK_END);
    str_sz = ftell(fp);
    rewind(fp);

    buf = (char*) malloc(sizeof(char) * (str_sz + 1) );

    fread(buf, sizeof(char), str_sz, fp);

    buf[str_sz] = '\0';
    fclose(fp);
  }
 
  struct toml_node *root;

  toml_init(&root);
  toml_parse(root, buf, strlen(buf));
  toml_node_walker fn = &ubqt_find_tables;
  
  toml_walk(root, fn, NULL);

  toml_free(root);
  free(buf);
  
  /* Set up windows */
  ubqt_initialize(argv[0]);
  
  /* Interactive loop */
  ubqt_run_loop();

  ubqt_destroy();

  return 0;
}
