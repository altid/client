# ubqt version
VERSION = 0.0

# Customize below to fit your system
TRANSPORT = local
BACK = cairo
SEAT = vi

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib
# includes and libs
INCS =
LIBS = -lpthread

# flags
CPPFLAGS := -D_FORTIFY_SOURCE=2 -O2
CFLAGS := -std=c99 -Wall ${INCS}
LDFLAGS := -s ${LIBS}

CC = clang -rpath /usr/lib
