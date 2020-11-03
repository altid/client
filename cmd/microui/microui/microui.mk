microui%.o : microui%.c
	@$(CC) ${INCS} -Wall -pedantic `sdl2-config --libs` -framework OpenGL -o $@ $<
