#define _GNU_SOURCE
#include <locale.h>
#include <xkbcommon/xkbcommon-x11.h>
#include <xcb/xcb.h>
#include <xcb/xkb.h>
#include <cairo/cairo-xcb.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "../../src/ubqt.h"

enum {
	BORDER = 2
};

// Global variables :(
xcb_connection_t *c;
xcb_screen_t *screen;
xcb_window_t window;

cairo_t *cr;
cairo_surface_t *surface = NULL;

PangoLayout *layout;
PangoFontDescription *desc;

struct xkb_context *ctx;
struct xkb_keymap *keymap;
struct xkb_state *state;
uint8_t *first_xkb_event;
int32_t device_id;

int width = 800;
int height = 600;

char *
ubqt_draw_error(int err)
{

	return "To be implemented\n";

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

void
update_keymap()
{

	struct xkb_keymap *new_keymap = xkb_x11_keymap_new_from_device(ctx, c, device_id, 0);
	struct xkb_state *new_state = xkb_x11_state_new_from_device(new_keymap, c, device_id);

	xkb_state_unref(state);
	xkb_keymap_unref(keymap);
	keymap = new_keymap;
	state = new_state;

}

int
ubqt_draw_init(char *title)
{

	setlocale(LC_CTYPE, "");
	xcb_visualtype_t *visual;
	asprintf(&ubqt_win.input, "%s", " ‣ ");
	
	c = xcb_connect(NULL, NULL);
	int ret;
	uint32_t mask;
	uint32_t values[2];
	
	if (xcb_connection_has_error(c)) {
		fprintf(stderr, "xcb_connect error\n");
		return 1;
	}

	mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = 1;
	values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;

	screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
	window = xcb_generate_id(c);

	xcb_create_window(c, XCB_COPY_FROM_PARENT, window, screen->root, 20, 20, width, height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, mask, values);

	xcb_change_property(c, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(title), title);
	
	//TODO: Get user mapping to generate key

	xcb_map_window(c, window);

	visual = find_visual(c, screen->root_visual);

	if(visual == NULL) {
		fprintf(stderr, "xcb connect error\n");
		return 1;
	}

	surface = cairo_xcb_surface_create(c, window, visual, width, height);
	cr = cairo_create(surface);
	
	layout = pango_cairo_create_layout(cr);
	desc = pango_font_description_from_string("DejaVu Sans Mono 10");
	pango_layout_set_font_description(layout, desc);
	pango_font_description_free(desc);
	pango_layout_set_wrap(layout, PANGO_WRAP_WORD);

	ret = xkb_x11_setup_xkb_extension(c, XKB_X11_MIN_MAJOR_XKB_VERSION, XKB_X11_MIN_MINOR_XKB_VERSION, XKB_X11_SETUP_XKB_EXTENSION_NO_FLAGS, NULL, NULL, first_xkb_event, NULL);

	device_id = xkb_x11_get_core_keyboard_device_id(c);

	ctx = xkb_context_new(XKB_CONTEXT_NO_DEFAULT_INCLUDES | XKB_CONTEXT_NO_ENVIRONMENT_NAMES);
	
	update_keymap();

	enum {
		required_events =
            (XCB_XKB_EVENT_TYPE_NEW_KEYBOARD_NOTIFY |
             XCB_XKB_EVENT_TYPE_MAP_NOTIFY |
             XCB_XKB_EVENT_TYPE_STATE_NOTIFY),

        required_nkn_details =
            (XCB_XKB_NKN_DETAIL_KEYCODES),

        required_map_parts =
            (XCB_XKB_MAP_PART_KEY_TYPES |
             XCB_XKB_MAP_PART_KEY_SYMS |
             XCB_XKB_MAP_PART_MODIFIER_MAP |
             XCB_XKB_MAP_PART_EXPLICIT_COMPONENTS |
             XCB_XKB_MAP_PART_KEY_ACTIONS |
             XCB_XKB_MAP_PART_VIRTUAL_MODS |
             XCB_XKB_MAP_PART_VIRTUAL_MOD_MAP),

        required_state_details =
            (XCB_XKB_STATE_PART_MODIFIER_BASE |
             XCB_XKB_STATE_PART_MODIFIER_LATCH |
             XCB_XKB_STATE_PART_MODIFIER_LOCK |
             XCB_XKB_STATE_PART_GROUP_BASE |
             XCB_XKB_STATE_PART_GROUP_LATCH |
             XCB_XKB_STATE_PART_GROUP_LOCK),
    };

    static const xcb_xkb_select_events_details_t details = {
        .affectNewKeyboard = required_nkn_details,
        .newKeyboardDetails = required_nkn_details,
        .affectState = required_state_details,
        .stateDetails = required_state_details,
    };

    xcb_void_cookie_t cookie = xcb_xkb_select_events_aux_checked(c, device_id, required_events, 0, 0, required_map_parts, required_map_parts, &details);

    xcb_generic_error_t *error = xcb_request_check(c, cookie);
    if (error) {
        free(error);
        return -1;
    }
	
	return 0;

}

int
ubqt_draw_destroy()
{

	xkb_state_unref(state);
	xkb_keymap_unref(keymap);
	g_object_unref(layout);
	cairo_destroy(cr);
	cairo_surface_finish(surface);
	xcb_disconnect(c);
	return 0;

}

void
ubqt_do_draw(cairo_t *cr, int x, int y)
{

	cairo_save (cr);
	cairo_move_to(cr, x, y);
	pango_cairo_show_layout(cr, layout);
	cairo_restore(cr);

}


void
ubqt_draw()
{
	
	pango_layout_set_width(layout, width * PANGO_SCALE);
	pango_layout_set_height(layout, height * PANGO_SCALE);
	PangoRectangle *ink = malloc(sizeof(PangoRectangle));
	PangoRectangle *logical = malloc(sizeof(PangoRectangle));

	// bg #262626
	cairo_set_source_rgb(cr, 0.148, 0.148, 0.148);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);

	// fg #bcbcbc
	cairo_set_source_rgb(cr, 0.73, 0.73, 0.73);

	/* We need a local representation of the remaining surface */
	int x = BORDER, y = BORDER, h = height - BORDER, w = width - BORDER;

	//TODO: Draw cursor in appropriate if'd section
	if (ubqt_win.title != NULL) {
		pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.title, strlen(ubqt_win.title));
		pthread_mutex_unlock(&mutex);
		pango_layout_get_pixel_extents(layout, ink, logical);
		ubqt_do_draw(cr, x, y);
		y += logical->height / 2;
		pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
	}
	

	if (ubqt_win.sidebar != NULL) {
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.sidebar, strlen(ubqt_win.sidebar));
		pthread_mutex_unlock(&mutex);
		ubqt_do_draw(cr, x, y);
		pango_layout_get_pixel_extents(layout, ink, logical);
		x += (logical->width) > w / 2 ? w / 2 : logical->width + (BORDER * 2);
		pango_layout_set_width(layout, (w - x) * PANGO_SCALE);
		//TODO: Draw a bar here to seperate
	}

	if (ubqt_win.tabs != NULL) {
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.tabs, strlen(ubqt_win.tabs));
		pthread_mutex_unlock(&mutex);
		ubqt_do_draw(cr, x, y);
		pango_layout_get_pixel_extents(layout, ink, logical);
		y += logical->height / 2;
	}

	if (ubqt_win.input != NULL) {
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.input, strlen(ubqt_win.input));
		pthread_mutex_unlock(&mutex);
		pango_layout_get_pixel_extents(layout, ink, logical);
		h -= logical->height;
		ubqt_do_draw(cr, x, h);

	}

	if (ubqt_win.status != NULL) {
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.status, strlen(ubqt_win.status));
		pthread_mutex_unlock(&mutex);
		pango_layout_get_pixel_extents(layout, ink, logical);
		h -= logical->height / 2;
		ubqt_do_draw(cr, x, h);
	}

	if (ubqt_win.main != NULL) {
		//TODO: Flag option to toggle pango_layout_set_justify(layout, true);
		pthread_mutex_lock(&mutex);
		pango_layout_set_markup(layout, ubqt_win.main, strlen(ubqt_win.main));
		pthread_mutex_unlock(&mutex);
		ubqt_do_draw(cr, x, y);
	}
	
	free(ink);
	free(logical);
	
	cairo_surface_flush(surface);
	xcb_flush(c);

}

int
ubqt_draw_new_data_callback()
{

	/* Make sure we zero out the message first */
	xcb_client_message_event_t ev;
	memset(&ev, 0, sizeof(xcb_client_message_event_t));

	ev.response_type = XCB_CLIENT_MESSAGE;
	ev.format = 32;

	xcb_send_event(c, 0, window, XCB_EVENT_MASK_NO_EVENT, (const char *)&ev);
	xcb_flush(c);

	return UBQT_SUCCESS;

}

int
ubqt_keypress_event(xcb_generic_event_t *e)
{

	//TODO: Handle modmasks
	size_t size = 0;
	xcb_key_press_event_t *ev;
	xkb_keycode_t keycode;

	ev = (xcb_key_press_event_t *)e;
	keycode = ev->detail;

	char *buffer = malloc(xkb_state_key_get_utf8(state, keycode, NULL, size) + 1);
	xkb_state_key_get_utf8(state, keycode, buffer, sizeof(buffer));

	if(ubqt_input_handle(buffer))
		return 1;

	free(buffer);
	return 0;
}

void
ubqt_resize_event(xcb_generic_event_t *e)
{

	xcb_expose_event_t *ev;
	ev = (xcb_expose_event_t *)e;
	width = ev->width;
	height = ev->height;
	cairo_xcb_surface_set_size(surface, width, height);

}

int
ubqt_draw_loop()
{

	ubqt_draw();

	xcb_generic_event_t *e;
	int done = 0;

	while(!done) {

		e = xcb_wait_for_event(c);

		switch(e->response_type & ~0x80) {
			case XCB_EXPOSE:
				ubqt_resize_event(e);
				ubqt_draw();
				break;

			case XCB_KEY_PRESS:
				done = ubqt_keypress_event(e);
				ubqt_draw();
				break;

			case XCB_CLIENT_MESSAGE:
			case XCB_KEY_RELEASE:
				ubqt_draw();
				break;
		}
	}

	return 0;

}
