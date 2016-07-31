#include "ubiquitous.h"
#include <jansson.h>
#include <string.h>

// parse json from $prog.json
// initialise the screen/terminal
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

  json_t *json, *data, *input, *menu, *buffer;
  json_error_t error;
  json = json_load_file(argv[1], 0, &error);

  if (!json)
    fprintf(stderr, "Error loading %s: %s %d\n", argv[1], error.text,
            error.line);

  if(menu = json_object_get(json, "menu"))
    draw_elements(menu);
  if(input = json_object_get(json, "input"))
    draw_elements(menu);
  if(buffer = json_object_get(json, "buffer"))
    draw_elements(buffer);

  initialize(argv[0]);

  destroy();
  return 0;
}
