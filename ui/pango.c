#include "../ubiquitous.h"
#include <pango/pangocairo.h>

cairo_t *cr;
cairo_surface_t *surface;

int height = 0;
int width = 0;

void
initialize() {
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, window, height);
  cr = cairo_create(surface);
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint(cr);
}

void
set_title(char *title) {
  (void) title;
}

void
navigate_back() {

}

void 
navigate_forward() {

}

void
destroy() {
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}
