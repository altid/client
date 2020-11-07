OBJ += draw/draw.o draw/microui.o draw/sdl.o
CFLAGS += 
LDFLAGS += `sdl2-config --libs` -framework OpenGL -lm

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
    LDFLAGS += -framework OpenGL
else
	LDFLAGS += -lGL
endif 

draw%.o : draw%.c
	@$(CC) ${INCS} -std=c11 -Wall -O3 -pedantic -g `sdl2-config --cflags` -c $< -o $@
