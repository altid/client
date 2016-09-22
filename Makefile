CC = clang
CFLAGS =
INCLUDES = -I/usr/local/include -L/usr/local/lib
DEPS = src/ubqt.h
LIBS=-ltoml
TARGET = ubqt
SRC = src/ubqt.c src/notify.c shared/vi.c shared/win_data.c
OBJ = src/ubqt.o src/notify.o shared/vi.o shared/win_data.o

ifeq (ncurses,${BACKEND})
	LIBS += -lncurses -lpanel
	SRC += ui/ncurses.c
	OBJ += ui/ncurses.o
endif

ifeq (cairo,${BACKEND})
	CFLAGS += `pkg-config --cflags cairo-gl pangocairo glfw3 epoxy`
	LIBS += `pkg-config --libs cairo-gl pangocairo glfw3 epoxy`
	SRC += ui/cairo.c
	OBJ += ui/cairo.o
endif

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(INCLUDES) -o $(TARGET) $(CFLAGS) $(LIBS) $(OBJ)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJ) *~ $(TARGET)

depend: $(SRC)
	makedepend $(INCLUDES) $^
