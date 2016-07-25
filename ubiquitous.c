#include <string.h>
#include <jansson.h>
#include "ubiquitous.h"


// parse json from $prog.json
// read in backend variable 
// initialise the screen/terminal
// connect filesystem in/out to UI elements
// run main loop, listening for key events or filesystem events
//  - UI element interactions, etc

int
main(int argc, char *argv[]) {
  (void) argc;
  initialize();
  set_title(argv[0]);
  destroy();
  return 0;
}
