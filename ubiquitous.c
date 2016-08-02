#include <unistd.h>
#include <jansson.h>
#include <string.h>

#include "ubiquitous.h"

void
ub_element_init(const char *key) 
{
  // Set up the element's available sizes, draw the background
}

void 
ub_element_build(json_t *json) 
{
  size_t index;
  json_t *value;
  json_array_foreach(json, index, value) {
  // set up each sub-element
  }
}

void
ub_element_draw(const char *key)
{
  // draw the element to the window 
}

int
main(int argc, char* argv[])
{

  if (argc != 2) {
    fprintf(stderr, "usage: %s <foo>.json\n", argv[0]);
    return 2;
  }
  
  // initialize keybindings, background, window
  ub_initialize(argv[0]);
  
  const char *key;
  json_t *json, *value;
  json_error_t error;

  json = json_load_file(argv[1], 0, &error);

  if (!json)
    fprintf(stderr, "Error loading %s: %s %d\n", argv[1], error.text,
            error.line);


  json_object_foreach(json, key, value) {
    ub_element_init(key);
    ub_element_build(value);
    ub_element_draw(key);
  }
  
  // finalize and draw 
  ub_draw();
 
  // Handle inputs and update
  ub_run_loop();

  ub_destroy();
  return 0;
}
