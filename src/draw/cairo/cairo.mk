cairo: CFLAGS += `pkg-config --cflags pangocairo cairo-xcb xcb-keysyms` -I${X11INC} -I../../draw/cairo
cairo: LDFLAGS += `pkg-config --libs pangocairo cairo-xcb xkbcommon-x11` -L${X11LIB} -lX11
cairo: SRC += src/draw/cairo/cairo.c src/draw/cairo/pangomarkup.c
cairo: OBJ += src/draw/cairo/cairo.o src/draw/cairo/pangomarkup.o
cairo: VPATH := ../../draw/cairo
cairo: $(OBJ) src/ubqt.h config.mk ubqt
