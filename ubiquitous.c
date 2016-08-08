#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "ubiquitous.h"
#include <toml.h>

// Current directiory (buffer) ~/ii/irc.freenode.net/##ubiquitous/out|in
typedef struct {
  FILE  *output;
  FILE  *input;
} Buffer;

typedef struct {
  char  *command;
  FILE  *in_file;
} Menu;

typedef struct {
  char  *title;
  Menu  menu;
  //navigation
  Buffer cur;
} Window;

void
ub_find_tables(struct toml_node *node, void *ctx) {
  (void) ctx;
  if(toml_type(node) == TOML_TABLE) {
    // Build struct of type
    //if (strcmp("menu", toml_name(node)))
      
  }
}

int
main(int argc, char* argv[])
{

  if (argc != 2) {
    fprintf(stderr, "usage: %s %s.toml\n", argv[0], argv[0]);
    return 2;
  }

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
  toml_node_walker fn = &ub_find_tables;
  
  toml_walk(root, fn, NULL);

  toml_free(root);
  free(buf);
  
  /* Set up windows */
  ub_initialize(argv[0]);
  
  /* Interactive loop */
  ub_run_loop();

  ub_destroy();
  return 0;
}
