# ubqt version
VERSION = 0.0

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# includes and libs
INCS = -I/usr/local/include
LIBS = -L/usr/lib -lc -lpthread

# flags
CFLAGS = -std=c99 -pedantic -Wall -Os ${INCS}
LDFLAGS = -s ${LIBS}

CC = clang
