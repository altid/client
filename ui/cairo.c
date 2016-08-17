#include <cairo-xcb.h>
#include <pango/pangocairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>

#include "../ubiquitous.h"

xcb_connection_t* c;
cairo_surface_t* surface;
cairo_t* cr;

static xcb_visualtype_t*
find_visual(xcb_connection_t* c, xcb_visualid_t visual)
{
  xcb_screen_iterator_t screen_iter =
    xcb_setup_roots_iterator(xcb_get_setup(c));

  for (; screen_iter.rem; xcb_screen_next(&screen_iter)) {
    xcb_depth_iterator_t depth_iter =
      xcb_screen_allowed_depths_iterator(screen_iter.data);
    for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
      xcb_visualtype_iterator_t visual_iter =
        xcb_depth_visuals_iterator(depth_iter.data);
      for (; visual_iter.rem; xcb_visualtype_next(&visual_iter))
        if (visual == visual_iter.data->visual_id)
          return visual_iter.data;
    }
  }
  return NULL;
}

void
ub_initialize(char* title)
{
  xcb_screen_t* screen;
  xcb_window_t window;
  xcb_visualtype_t* visual;

  c = xcb_connect(NULL, NULL);

  uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  uint32_t values[2] = { screen->white_pixel, XCB_EVENT_MASK_EXPOSURE };
  screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;
  window = xcb_generate_id(c);
  xcb_create_window(c, XCB_COPY_FROM_PARENT, window, screen->root, 0, 0, 400,
                    400, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual,
                    mask, values);

  xcb_change_property(c, XCB_PROP_MODE_REPLACE, window, XCB_ATOM_WM_NAME,
                      XCB_ATOM_STRING, 8, strlen(title), title);

  xcb_map_window(c, window);

  visual = find_visual(c, screen->root_visual);
  surface = cairo_xcb_surface_create(c, window, visual, 400, 400);
  cr = cairo_create(surface);

  xcb_flush(c);
  cairo_surface_finish(surface);
  
}

void
ub_draw() {
  int width, height;

  PangoLayout *layout;
  PangoFontDescription *desc;


  layout = pango_cairo_create_layout(cr);
  pango_layout_set_text(layout, "Hello World!", -1);
  desc = pango_font_description_from_string("Sans Bold 12");
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);  
  
  pango_layout_get_size(layout, &width, &height);
  cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
  cairo_rectangle(cr, 0, 0, width, 12);
  cairo_fill(cr);

  cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
  pango_cairo_update_layout(cr, layout);
  cairo_move_to(cr, 0, 0);

  pango_cairo_show_layout(cr, layout);

  g_object_unref(layout);

}

void
ub_run_loop() {
  xcb_flush(c);
  xcb_generic_event_t* event;
  while ((event = xcb_wait_for_event(c)) != NULL) {
    switch (event->response_type & ~0x80) {
      case XCB_EXPOSE:
        ub_draw();
        break;
    }
    free(event);
  }
}
void
ub_destroy()
{
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
  xcb_disconnect(c);
}

void
ub_menu_save(char *t)
{
  win.menu[SAVE] = t;
}

void
ub_menu_quit(char *t)
{
  win.menu[QUIT] = t;
}

void
ub_buffer_in(char *t)
{

}

void
ub_navigation(char *t)
{

}
