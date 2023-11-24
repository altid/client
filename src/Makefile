CFLAGS += -D_REENTRANT
CFLAGS += -Wall -O3
CFLAGS += -fno-strict-aliasing
CFLAGS += -I../include
LDFLAGS = 

.PHONY:	all clean

all:	test

queue.o: 
	$(CC) $(CFLAGS) -c -o queue.o queue.c

test.o: queue.h
	$(CC) $(CFLAGS) -c -o test.o test.c

test: queue.o test.o $(TMILB)
	$(CC) $(CFLAGS) queue.o  test.o -o test $(LDFLAGS)

clean:
	-rm -f *.o test
