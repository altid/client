CC = clang -rpath /usr/lib
CFLAGS =
INCLUDES =
DEPS = src/ubqt.h
LIBS = -lpthread
TARGET = ubqt
SRC = src/ubqt.c src/notify.c src/data.c src/connection.c
OBJ = src/ubqt.o src/notify.o src/data.o src/connection.o


ifeq (ncurses,${BACKEND})
	LIBS += -lncurses -lpanel
	SRC += draw/ncurses/ncurses.c
	OBJ += draw/ncurses/ncurses.o
else
	CFLAGS += `pkg-config --cflags cairo-gl pangocairo glfw3 epoxy`
	LIBS += `pkg-config --libs cairo-gl pangocairo glfw3 epoxy`
	SRC += draw/cairo/cairo.c draw/cairo/pangomarkup.c
	OBJ += draw/cairo/cairo.o draw/cairo/pangomarkup.o
	DEPS += draw/cairo/pangomarkup.h
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(INCLUDES) -o $(TARGET) $(CFLAGS) $(LIBS) $(OBJ)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJ) *~ $(TARGET) ui/*.o

depend: $(SRC)
	makedepend $(INCLUDES) $^
