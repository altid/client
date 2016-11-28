# ubqt - simple frontend for file server-based programs
# See LICENSE for copyright and license details.

include config.mk
include draw/cairo/cairo.mk
include draw/ncurses/ncurses.mk
include draw/nuklear/nuklear.mk
include input/vi/vi.mk
include input/leanback/leanback.mk

SRC = src/ubqt.c src/data.c src/connection.c src/notify.c
OBJ = src/ubqt.o src/data.o src/connection.o src/notify.o

# By default, use cairo backend
all: options

options:
	@echo ubqt build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"
	@echo "Usage: make <backend>"
	@echo "Backends = cairo"
	@echo "         = ncurses"
	@echo "         = nuklear"
	@echo "(input)  = vi"
	@echo "         = game"

.c.o:
	@echo CC $<
	$(CC) -c ${CFLAGS} $<

${OBJ}: src/ubqt.h config.mk

ubqt: ${OBJ}
	@${CC} -o $@ ${CFLAGS} ${LDFLAGS}

dist: clean
	@echo creating dist tarball
	@mkdir -p ubqt-${VERSION}
	@cp -R LICENSE Makefile README.md ubqt.1 ${SRC} ${OBJ} ubqt-${VERSION}
	@tar -cf ubqt-${VERSION}.tar ubqt-${VERSION}
	@gzip ubqt-${VERSION}.tar
	@rm -rf ubqt-${VERSION}

clean:
	@echo cleaning
	@rm -rf $(OBJ) *~ $(TARGET) draw/*/*.o

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f ubqt ${DESTDIR}${PREFIX}/bin/ubqt
	@chmod 755 ${DESTDIR}${PREFIX}/bin/ubqt
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/c" < ubqt.1 > ${DESTDIR}${MANPREFIX}/man1/ubqt.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/ubqt.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/ubqt
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/ubqt.1

.PHONY: all options clean dist install uninstall
