# ubqt version
VERSION = 0.0

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# includes and libs
INCS = -I/usr/lib -I/usr/local/lib64
LIBS = -lpthread -L/usr/lib -L/usr/local/lib

# flags
CPPFLAGS += -D_FORTIFY_SOURCE=2
CFLAGS += -std=c99 -Wall -O3 -fPIE ${INCS}
LDFLAGS += -s ${LIBS}

CC = clang -rpath /usr/lib
