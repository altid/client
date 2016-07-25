#include <string.h>
#include <jansson.h>

#ifdef PANGO
#include "ui/pango.h"
#elif NUKLEAR
#include "ui/nuklear.h"
#else
#include "ui/ncurses.h"
#endif

// parse json from $prog.json
// read in backend variable 
// initialise the screen/terminal
// connect filesystem in/out to UI elements
// run main loop, listening for key events or filesystem events
//  - UI element interactions, etc
