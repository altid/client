# Dummy function 

ncurses-mk:
	$(eval LIBS += `pkg-config --libs ncursesw`)
	$(eval OBJ += draw/ncurses/ncurses.o)
	$(eval SRC += draw/ncurses/ncurses.c)

ncurses: ncurses-mk ubqt

.PHONY: ncurses ncurses-mk
