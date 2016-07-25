#include "../ubiquitous.h"
#include <string.h>
#include <pango/pangocairo.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <cairo-xcb.h>
#include <stdio.h>
#include <stdlib.h>
	
xcb_connection_t *c;

static 
xcb_visualtype_t *find_visual(xcb_connection_t *c, xcb_visualid_t visual) {
	xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(xcb_get_setup(c));
	
	for (; screen_iter.rem; xcb_screen_next(&screen_iter)) {
		xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen_iter.data);
		for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
			xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
			for (; visual_iter.rem; xcb_visualtype_next(&visual_iter))
				if (visual == visual_iter.data->visual_id)
					return visual_iter.data;
		}
	}
	return NULL;
}

void
initialize() {
	xcb_screen_t *screen;
	xcb_window_t window;
	uint32_t mask[2];
	xcb_visualtype_t *visual;
	xcb_generic_event_t *event;
	cairo_surface_t *surface;
	cairo_t *cr;

	c = xcb_connect(NULL, NULL);

	mask[0] = 1;
	mask[1] = XCB_EVENT_MASK_EXPOSURE;
	screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
	window = xcb_generate_id(c);
	xcb_create_window(c, XCB_COPY_FROM_PARENT, window, screen->root,
			0, 0, 400, 400, 0,
			XCB_WINDOW_CLASS_INPUT_OUTPUT,
			screen->root_visual,
			XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK,
			mask);

  char *title = "test";
  xcb_change_property(c, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, 
      XCB_ATOM_STRING, 8, strlen(title), title);

	xcb_map_window(c, window);

	visual = find_visual(c, screen->root_visual);
	surface = cairo_xcb_surface_create(c, window, visual, 400, 400);
	cr = cairo_create(surface);

	xcb_flush(c);
	while ((event = xcb_wait_for_event(c))) {
		switch (event->response_type & ~0x80) {
		case XCB_EXPOSE:
			/* Should check if this is the last expose event in the
			 * sequence, but I'm too lazy right now...
			 */
			cairo_set_source_rgb(cr, 0, 1, 0);
			cairo_paint(cr);

			cairo_set_source_rgb(cr, 1, 0, 0);
			cairo_move_to(cr, 0, 0);
			cairo_line_to(cr, 150, 0);
			cairo_line_to(cr, 150, 150);
			cairo_close_path(cr);
			cairo_fill(cr);

			cairo_set_source_rgb(cr, 0, 0, 1);
			cairo_set_line_width(cr, 20);
			cairo_move_to(cr, 0, 150);
			cairo_line_to(cr, 150, 0);
			cairo_stroke(cr);

			cairo_surface_flush(surface);
			break;
		}
		free(event);
		xcb_flush(c);
	}
  cairo_surface_finish(surface);
}
 
void
destroy() {
	xcb_disconnect(c);
}
