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
#include <stdlib.h>
#include <stdbool.h>
#include "../src/ubqt.h"
#include "../shared/vi.h"

// Global variables :(
GLFWwindow *window;
cairo_surface_t* surface = NULL;
cairo_t *cr;
int width = 1000;
int height = 1000;

// GLFW callbacks
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}
void resize_callback(GLFWwindow* window, int nwidth, int nheight) {
  width = nwidth;
  height = nheight;
  cairo_gl_surface_set_size(surface,width,height);
  glViewport(0, 0, width, height);
  glScissor(0, 0, width, height);
  printf("Resize: %d, %d\n", width, height);
}

void do_exit(int code) {
  glfwTerminate();
  //exit(code);
}

void 
ubqt_initialize(char *title) {
  glfwSetErrorCallback(error_callback);

  /* Initialize the library */
  //if (!glfwInit()) return -1;
  glfwInit();

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(1000, 1000, "Hello", NULL, NULL);
  if (!window) { do_exit(-1); }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  cairo_device_t* device = cairo_glx_device_create(glfwGetX11Display(),glfwGetGLXContext(window));

  glfwSetKeyCallback(window, key_callback);

  glfwSetFramebufferSizeCallback(window,resize_callback);
  glfwGetFramebufferSize(window, &width, &height);
  surface = cairo_gl_surface_create_for_window(device, glfwGetX11Window(window), width, height);

}

void ubqt_destroy() {
  glfwDestroyWindow(window);
  printf("Bye!\n");
  glfwTerminate();
}


static void draw_text(cairo_t* cr, char* font) {
  char *buf = NULL;
  FILE *fp;
  int str_sz;
  fp = fopen(ubqt_win.current_out, "r");

  if(fp)
  {
    fseek(fp, 0, SEEK_END);
    str_sz = ftell(fp);
    rewind(fp);
    
    buf = (char*) malloc(sizeof(char) * (str_sz + 1));

    fread(buf, sizeof(char), str_sz, fp);

    buf[str_sz] = '\0';
    fclose(fp);
  } 

  PangoLayout *layout;
  PangoFontDescription *desc;

  /* Create a PangoLayout, set the font and text */
  layout = pango_cairo_create_layout (cr);

  pango_layout_set_text(layout, buf, -1);
  desc = pango_font_description_from_string(font);
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);

  int width, height;
  cairo_save (cr);

  /* Inform Pango to re-layout the text with the new transformation */
  pango_cairo_update_layout (cr, layout);
  
  pango_layout_get_size (layout, &width, &height);
  pango_cairo_show_layout (cr, layout);
  
  cairo_restore (cr);

  /* free the layout object */
  g_object_unref (layout);
}

void
ubqt_handle_keypress() {

}

void
ubqt_update_buffer() {
  
  cr = cairo_create(surface);
  // bg #262626
  cairo_set_source_rgb(cr, 0.148, 0.148, 0.148);
  cairo_rectangle(cr, 0, 0, width, height);
  cairo_fill(cr);

  // fg #bcbcbc
  cairo_set_source_rgb(cr, 0.73, 0.73, 0.73);
  cairo_move_to(cr, 3, 3);
  draw_text(cr,"DejaVu Sans Mono 8");

}

void
ubqt_update_input() {

}

void
ubqt_refresh() {
  cairo_surface_flush(surface);

  cairo_gl_surface_swapbuffers(surface);

  glfwSwapBuffers(window);
  glClear(GL_COLOR_BUFFER_BIT);

  cairo_destroy(cr);

}
