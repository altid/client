# altid version
VERSION = 0.0


# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# includes and libs
INCS = -Iinclude
LIBS =

# flags
CPPFLAGS := -D_FORTIFY_SOURCE=2
CFLAGS := -std=c99 -Wall ${INCS}
LDFLAGS := ${LIBS}

CC = cc
