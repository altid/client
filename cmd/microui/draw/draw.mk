DRAWLIB= 				\
	draw/microui.h		\
	draw/render.h		\
	include/draw.h		\

draw/draw.a : draw/sdl.c draw/draw.c draw/microui.c
	ar -csr draw/draw.a draw/sdl.o draw/draw.o draw/microui.o
	ranlib draw/draw.a

# This should be globbed in the future
draw/%.o : draw/%.c
	@$(CC) ${INCS} ${DRAWLIB} -Wall -pedantic `sdl2-config --libs` -framework OpenGL -c *.c
