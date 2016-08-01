#include "ubiquitous.h"
#include <jansson.h>
#include <string.h>

// connect filesystem in/out to UI elements
//
// run main loop, listening for key events or filesystem events

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
    fprintf(stderr, "Error loading %s: %s %d\n", argv[1], error.text,
            error.line);


  /* In each element, run top level function, with argument of sub-section of json */
  json_object_foreach(json, key, value) {
  }

  ub_initialize(argv[0]);

  ub_destroy();
  return 0;
}
