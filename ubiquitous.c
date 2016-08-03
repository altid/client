#include <stdio.h>
#include <jansson.h>
#include <string.h>
#include <stdbool.h>

#include "ubiquitous.h"

// Current directiory (buffer) ~/ii/irc.freenode.net/##ubiquitous/out|in
typedef struct {
  FILE  *buf;
  FILE  *in;
} Buffer;

typedef struct {
  //menu items
  //navigation
  //titlestring
  //input text
  Buffer cur;
} State;

int
main(int argc, char* argv[])
{

  if (argc != 2) {
    fprintf(stderr, "usage: %s <foo>.json\n", argv[0]);
    return 2;
  }
  
  const char *key;
  json_t *json, *value;
  json_error_t error;

  json = json_load_file(argv[1], 0, &error);

  if (!json)
    fprintf(stderr, "Error loading %s: %s %d\n", argv[1], error.text, error.line);

  /* Load structs */
  json_object_foreach(json, key, value) {
    printf("%s\n", key);
  }
  
  /* Set up windows */
  ub_initialize(argv[0]);
  
  /* Interactive loop */
  ub_run_loop();

  ub_destroy();
  return 0;
}
