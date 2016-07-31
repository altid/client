#include "ubiquitous.h"
#include <jansson.h>
#include <string.h>

// connect filesystem in/out to UI elements
//
// run main loop, listening for key events or filesystem events

void
draw_elements(json_t *menu)
{
  printf("%zu\n", json_array_size(menu));
}

int
main(int argc, char* argv[])
{

  if (argc != 2) {
    fprintf(stderr, "usage: %s <foo>.json\n", argv[0]);
    return 2;
  }

  json_t *json, *menu, *input, *buffer, *title, *navigation;
  json_error_t error;
  json = json_load_file(argv[1], 0, &error);

  if (!json)
    fprintf(stderr, "Error loading %s: %s %d\n", argv[1], error.text,
            error.line);

  if((menu = json_object_get(json, "menu")))
    draw_elements(menu);
  if((input = json_object_get(json, "input")))
    draw_elements(input);
  if((title = json_object_get(json, "title")))
    draw_elements(title);
  if((buffer = json_object_get(json, "buffer")))
    draw_elements(buffer);
  if((navigation = json_object_get(json, "navigation")))
    draw_elements(navigation);
  
  initialize(argv[0]);

  destroy();
  return 0;
}
