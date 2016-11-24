// Have a bunch of surfaces that we hold on to, then stitch together as we need them; should save a bunch of resources
// Surface holds everything that we need
#include <epoxy/glx.h>
#include <GLFW/glfw3.h>
// We have to do this for glfw-cairo interop :(
#define GLFW_EXPOSE_NATIVE_X11 1
#define GLFW_EXPOSE_NATIVE_GLX 1
#include <GLFW/glfw3native.h>
#include <cairo.h>
#include <cairo-gl.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../src/ubqt.h"

// Global variables :(
GLFWwindow *window;
cairo_t *cr;
cairo_device_t *device = NULL;
cairo_surface_t *surface = NULL;
int width = 800;
int height = 600;

// GLFW callbacks
static void 
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void 
error_callback(int error, const char* description) {
		fprintf(stderr, "Error: %s\n", description);
}
void 
resize_callback(GLFWwindow* window, int nwidth, int nheight) {
		width = nwidth;
		height = nheight;
		cairo_gl_surface_set_size(surface, width, height);
		glViewport(0, 0, width, height);
		glScissor(0, 0, width, height);
}

void 
ubqt_window_init() {
		glfwSetErrorCallback(error_callback);
		glfwInit();
		window = glfwCreateWindow(width, height, "Hello", NULL, NULL);
		if (!window)
				glfwTerminate();

		glfwMakeContextCurrent(window);
		
		device = cairo_glx_device_create(glfwGetX11Display(),glfwGetGLXContext(window));
		glfwGetFramebufferSize(window, &width, &height);
		surface = cairo_gl_surface_create_for_window(device, glfwGetX11Window(window), width, height);
		cairo_device_destroy(device);
		
		glfwSetFramebufferSizeCallback(window,resize_callback);
		glfwSetKeyCallback(window, key_callback);
		
}

void 
ubqt_destroy() {
		glfwDestroyWindow(window);
		printf("Bye!\n");
		glfwTerminate();
}

char *
ubqt_toxml(char *md, int width, int height) {
		return md;
}

void
ubqt_draw(cairo_t *cr) {
		/* We need a local representation of the remaining surface */
		int x = 3, y = 3, w = width - 3, h = height - 3;
		char *buf;
		PangoLayout *layout = pango_cairo_create_layout(cr);
		
		PangoFontDescription *desc;
		desc = pango_font_description_from_string("DejaVu Sans Mono 8");
		pango_layout_set_font_description(layout, desc);
		pango_font_description_free(desc);
		
		pango_layout_set_wrap(layout, PANGO_WRAP_WORD);
		pango_layout_set_width(layout, width * PANGO_SCALE);
		pango_layout_set_height(layout, height * PANGO_SCALE);
		
		if (ubqt_win.title != NULL) {
				buf = ubqt_toxml(ubqt_win.title, w, h);
				pango_layout_set_markup(layout, buf, strlen(buf));
				cairo_save (cr);
				cairo_move_to(cr, x, y);
				pango_cairo_show_layout (cr, layout);
				cairo_restore(cr);
				// pango get height of layout for these values
				y += 14;
				pango_layout_set_height(layout, h * PANGO_SCALE);
		}

		if (ubqt_win.tabbar != NULL) {
				buf = ubqt_toxml(ubqt_win.tabbar, w, h);
				pango_layout_set_markup(layout, buf, strlen(buf));	
				cairo_save(cr);
				cairo_move_to(cr, x, y);
				pango_cairo_show_layout(cr, layout);
				cairo_restore(cr);
				y += 14;
				pango_layout_set_height(layout, (h - y) * PANGO_SCALE);
		}		
		if (ubqt_win.status != NULL) {
				buf = ubqt_toxml(ubqt_win.status, w, h);
				pango_layout_set_markup(layout, buf, strlen(buf));
				cairo_save(cr);
				cairo_move_to(cr, x, h - 14);
				pango_cairo_show_layout(cr, layout);
				cairo_restore(cr);
				h -=14;
				pango_layout_set_height(layout, (h - y) * PANGO_SCALE);
		}
		if (ubqt_win.input != NULL) {
				buf = ubqt_toxml(ubqt_win.input, w, h);
				pango_layout_set_markup(layout, buf, strlen(buf));
				cairo_save(cr);
				pango_cairo_show_layout(cr, layout);
				cairo_move_to(cr, x, h - 14);
				h -= 14;
				pango_layout_set_height(layout, (h - y) * PANGO_SCALE);
		}
		if (ubqt_win.sidebar != NULL) {
				buf = ubqt_toxml(ubqt_win.sidebar, w, h);
				pango_layout_set_markup(layout, buf, strlen(buf));
				cairo_save(cr);
				pango_cairo_show_layout(cr, layout);
				cairo_move_to(cr, x, y);
				y += 14;
				pango_layout_set_width(layout, (w - x) * PANGO_SCALE);
		}
		if (ubqt_win.text != NULL) {		
				buf = ubqt_toxml(ubqt_win.text, w, h);
				pango_layout_set_markup(layout, buf, strlen(buf));
				cairo_save(cr);
				cairo_move_to(cr, x, y);
				pango_cairo_show_layout(cr, layout);
				cairo_restore(cr);
		}

		g_object_unref(layout);
}

void
ubqt_update_buffer() {
		glfwPostEmptyEvent();
		
}

void
ubqt_main_loop() {
		while (!glfwWindowShouldClose(window)) {
				glClear(GL_COLOR_BUFFER_BIT);
		
				cr = cairo_create(surface);
		
				// bg #262626
				cairo_set_source_rgb(cr, 0.148, 0.148, 0.148);
				cairo_rectangle(cr, 0, 0, width, height);
				cairo_fill(cr);

				// fg #bcbcbc
				cairo_set_source_rgb(cr, 0.73, 0.73, 0.73);
		
				ubqt_draw(cr);

				cairo_gl_surface_swapbuffers(surface);
				
				cairo_destroy(cr);
				
				glfwWaitEvents();
		}
}
