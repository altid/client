cairo: CFLAGS += `pkg-config --cflags pangocairo cairo-xcb xcb-keysyms` -I${X11INC}
cairo: LDFLAGS += `pkg-config --libs pangocairo cairo-xcb xkbcommon-x11 xcb-xkb` -L${X11LIB} -lX11
cairo: SRC += draw/cairo/cairo.c draw/cairo/pangomarkup.c
cairo: OBJ += draw/cairo/cairo.o draw/cairo/pangomarkup.o
cairo: draw/cairo/cairo.o draw/cairo/pangomarkup.o ubqt
