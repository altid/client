c9%.o : c9%.c
	@$(CC) -std=c99 -fms-extensions -DC9_NO_SERVER -O0 -g -Wall -Wextra -Wshadow ${INCS} -o $@ $<