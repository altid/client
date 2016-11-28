# Cairo specific

cairo-mk:
	$(eval CFLAGS += `pkg-config --cflags pangocairo cairo-xcb xcb-keysyms`)
	$(eval LIBS += `pkg-config --libs pangocairo cairo-xcb xkbcommon-x11`)
	$(eval SRC += draw/cairo/cairo.c draw/cairo/pangomarkup.c)
	$(eval OBJ += draw/cairo/cairo.o draw/cairo/pangomarkup.o)

cairo: cairo-mk ubqt
