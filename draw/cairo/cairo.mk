# Cairo specific

.PHONY: cairo-mk
cairo-mk:
	$(eval CFLAGS += `pkg-config --cflags pangocairo cairo-xcb xcb-keysyms` -I${X11INC})
	$(eval LIBS += `pkg-config --libs pangocairo cairo-xcb xkbcommon-x11` -L${X11LIB} -lX11)
	$(eval SRC += draw/cairo/cairo.c draw/cairo/pangomarkup.c)
	$(eval OBJ += draw/cairo/cairo.o draw/cairo/pangomarkup.o)

.PHONY: cairo
cairo: cairo-mk ubqt
