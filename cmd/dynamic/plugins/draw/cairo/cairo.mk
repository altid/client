CFLAGS += `pkg-config --cflags pangocairo cairo-xcb xcb-keysyms` -I${X11INC}
LDFLAGS += `pkg-config --libs pangocairo cairo-xcb xkbcommon-x11 xcb-xkb` -L${X11LIB} -lX11
SRC += plugins/draw/cairo/draw.c plugins/draw/cairo/markup.c
OBJ += plugins/draw/cairo/draw.o plugins/draw/cairo/markup.o
