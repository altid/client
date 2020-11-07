OBJ += c9/c9.o c9/connect.o c9/buffer.o
CFLAGS +=
LDFLAGS += 
c9%.o : c9%.c
	@$(CC) ${INCS} -std=c99 -fms-extensions -DC9_NO_SERVER -Ofast -g -Wall -Wextra -Wshadow -c $< -o $@