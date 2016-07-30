#include <string.h>
#include <jansson.h>
#include "ubiquitous.h"

// parse json from $prog.json
// initialise the screen/terminal
// connect filesystem in/out to UI elements
//
// run main loop, listening for key events or filesystem events

int
main(int argc, char *argv[]) {
  
  if (argc != 2) {
    fprintf(stderr, "usage: %s <%s.json>", argv[0], argv[0]);
    return 2;
  }

  json_t *json;
  json_error_t error;
  //const char *path = argv[1];  
  json = json_load_file(argv[1], 0, &error);
  
  if (!json)
    fprintf(stderr, "Error loading %s: %s %d\n", argv[1], error.text, error.line);
  

  initialize();
//  set_title(argv[0]);
  destroy();
  return 0;
}
