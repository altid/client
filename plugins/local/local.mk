local: CFLAGS += 
local: LDFLAGS +=
local: SRC += data/local/notify.c data/local/connection.c
local: OBJ += data/local/notify.o data/local/connection.o
local: data/local/notify.o data/local/connection.o ubqt
