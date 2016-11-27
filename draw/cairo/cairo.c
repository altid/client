// Surface holds everything that we need
#include <xcb/xcb.h>
#include <cairo-xcb.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "../../src/ubqt.h"

// Global variables :(
xcb_connection_t *c;
xcb_screen_t *screen;
xcb_window_t *window;
cairo_t *cr;
cairo_device_t *device = NULL;
cairo_surface_t *surface = NULL;
int width = 800;
int height = 600;

char *
ubqt_draw_error(int err)
{

	return "See scrollback for glfw error\n";

}

static xcb_visualtype_t *
find_visual(xcb_connection_t *c, xcb_visualid_t visual)
{
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
	
int
ubqt_draw_init(char *title)
{

	xcb_visualtype_t *visual;
	
	c = xcb_connect(NULL, NULL);
	uint32_t mask[2];
	
	if (xcb_connection_has_error(c)) { 
		fprintf(stderr, "xcb_connect error\n");
		return 1;
	}
	
	mask[0] = 1;
	mask[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;
	
	screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
	window = xcb_generate_id(c);

	xcb_create_window(c, XCB_COPY_FROM_PARENT, window, screen->root, 20, 20, width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, XCB_CW_OVERRIDE_REDIRECT | XCB_CW_EVENT_MASK, mask);

	xcb_change_property(c, XCB_PROP_MODE_REPLACE, window, WM_NAME, STRING, 8, strlen(title), title);

	xcb_map_window(c, window);
	
	visual = find_visual(c, screen->root_visual);
	
	if(visual == NULL) {
		fprintf(stderr, "xcb connect error\n");
		return 1;
	}

	surface = cairo_xcb_surface_create(c, window, visual, width, height);

	return 0;

}

int
ubqt_draw_destroy()
{

	cairo_surface_finish(surface);
	xcb_disconnect(c);
	return 0;

}

void
ubqt_draw(cairo_t *cr)
{

	/* We need a local representation of the remaining surface */
	int x = 3, y = 3, w = width - 3, h = height - 3;
	PangoLayout *layout = pango_cairo_create_layout(cr);

	PangoFontDescription *desc;
	desc = pango_font_description_from_string("DejaVu Sans Mono 8");
	pango_layout_set_font_description(layout, desc);
	pango_font_description_free(desc);

	pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
	pango_layout_set_width(layout, width * PANGO_SCALE);
	pango_layout_set_height(layout, height * PANGO_SCALE);

	//TODO: Push this to a seperate function
	if (ubqt_win.title != NULL) {
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.title, strlen(ubqt_win.title));
		pthread_mutex_unlock(&mutex);
		cairo_save (cr);
		cairo_move_to(cr, x, y);
		pango_cairo_show_layout (cr, layout);
		cairo_restore(cr);
		y += 14;
	}

	if (ubqt_win.sidebar != NULL) {
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.sidebar, strlen(ubqt_win.sidebar));
		pthread_mutex_unlock(&mutex);
		cairo_save(cr);
		pango_cairo_show_layout(cr, layout);
		cairo_move_to(cr, x, y);
		y += 14;
	}

	if (ubqt_win.tabbar != NULL) {
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.tabbar, strlen(ubqt_win.tabbar));
		pthread_mutex_unlock(&mutex);
		cairo_save(cr);
		cairo_move_to(cr, x, y);
		pango_cairo_show_layout(cr, layout);
		cairo_restore(cr);
		y += 14;
	}

	if (ubqt_win.status != NULL) {
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.status, strlen(ubqt_win.status));
		pthread_mutex_unlock(&mutex);
		cairo_save(cr);
		cairo_move_to(cr, x, h - 14);
		pango_cairo_show_layout(cr, layout);
		cairo_restore(cr);
		h -=14;
	}

	if (ubqt_win.input != NULL) {
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.input, strlen(ubqt_win.input));
		pthread_mutex_unlock(&mutex);
		cairo_save(cr);
		pango_cairo_show_layout(cr, layout);
		cairo_move_to(cr, x, h - 14);
		h -= 14;
	}

	if (ubqt_win.text != NULL) {
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.text, strlen(ubqt_win.text));
		pthread_mutex_unlock(&mutex);
		cairo_save(cr);
		cairo_move_to(cr, x, y);
		pango_cairo_show_layout(cr, layout);
		cairo_restore(cr);
	}

	g_object_unref(layout);

}

void
ubqt_update_buffer()
{

	

}

int
ubqt_draw_loop()
{

	xcb_flush(c);
	cr = cairo_create(surface);
	xcb_generic_event_t *event;
	
	while((event = xcb_wait_for_event(c))) {
		switch(event->response_type & ~0x80) {
			case XCB_KEY_PRESS: {
				xcb_key_press_event_t *ev = (xcb_key_press_event_t *)event;
				printf("key pressed: %c\n", ev->detail);
				break;
			}
			case XCB_EXPOSE: {

			// bg #262626
			cairo_set_source_rgb(cr, 0.148, 0.148, 0.148);
			cairo_rectangle(cr, 0, 0, width, height);
			cairo_fill(cr);

			// fg #bcbcbc
			cairo_set_source_rgb(cr, 0.73, 0.73, 0.73);

			ubqt_draw(cr);

			cairo_surface_flush(surface);
			break;
			}
		}

		free(event);
		xcb_flush(c);

	}

	return 0;

}
