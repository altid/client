CFLAGS += `pkg-config --cflags pangocairo cairo-xcb xcb-keysyms` -I${X11INC}
LDFLAGS += `pkg-config --libs pangocairo cairo-xcb xkbcommon-x11 xcb-xkb` -L${X11LIB} -lX11
SRC += plugins/cairo/cairo.c plugins/cairo/pangomarkup.c
OBJ += plugins/cairo/cairo.o plugins/cairo/pangomarkup.o
