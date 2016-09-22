#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>
#include "ubqt.h"

void
ubqt_toml_parse_tables(struct toml_node *node, void *ctx) {
  
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
    toml_node_walker fn = &ubqt_toml_parse_tables;
    char *name = toml_name(node);
    toml_walk(node, fn, name);
    free(name);
  }
}

void
ubqt_initialize(char *path) {
  char *buf = ubqt_read_buffer(path, 0, 0);
  
  struct toml_node *root;

  toml_init(&root);
  toml_parse(root, buf, strlen(buf));
  toml_node_walker fn = &ubqt_find_tables;
  toml_walk(root, fn, NULL);

  toml_free(root);
  free(buf);

}
