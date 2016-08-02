#include <unistd.h>
#include <jansson.h>
#include <string.h>

typedef union {
  int i;
  float f;
  const void *v;
} Arg;

typedef struct {
  char *k;
  void (*func)(const Arg *);
  const Arg arg;
} Func;

#include "ubiquitous.h"

void 
ub_element_build(json_t *json, const char *key) 
{
  (void) key;
  size_t index;
  json_t *value;
  json_array_foreach(json, index, value) {
    for(size_t i = 0; i < sizeof(ub_function); i++) {
      if(strcmp(ub_function[i].k, json_string_value(value))) {
        ub_function[i].func(&(ub_function[i].arg));
      }
    }
  }
}

int
main(int argc, char* argv[])
{

  if (argc != 2) {
    fprintf(stderr, "usage: %s <foo>.json\n", argv[0]);
    return 2;
  }
  
  ub_initialize(argv[0]);
  
  const char *key;
  json_t *json, *value;
  json_error_t error;
  json = json_load_file(argv[1], 0, &error);

  if (!json)
    fprintf(stderr, "Error loading %s: %s %d\n", argv[1], error.text,
            error.line);


  json_object_foreach(json, key, value) {
    ub_element_build(json_object_get(json, key), key);
  }

  ub_draw();
  
  ub_destroy();
  return 0;
}
