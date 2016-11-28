# ubqt - simple frontend for file server-based programs
# See LICENSE for copyright and license details.

SRC = src/ubqt.c src/data.c src/connection.c src/notify.c
OBJ = src/ubqt.o src/data.o src/connection.o src/notify.o

# Have make output usage with no args
options:
	@echo "Usage: make <backend> [<input>]"
	@echo "backends = cairo"
	@echo "         = ncurses"
	@echo "         = nuklear"
	@echo "(input)  = vi"
	@echo "         = game"

include config.mk
include */*/*.mk

include $(SRC:.c=.d)

%d: %.c
	@set -e; rm -f $@; \
		$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$

%.o: ${LDFLAGS}
	@echo CC $<
	$(CC) -c ${CFLAGS} $<

$(OBJ): src/ubqt.h

ubqt: ${OBJ} ./src/ubqt.h
	@echo CC -o $@
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

dist: clean
	@echo creating dist tarball
	@mkdir -p ubqt-$(VERSION)
	@cp -R LICENSE Makefile README.md ubqt.1 $(SRC) $(OBJ) ubqt-$(VERSION)
	@tar -cf ubqt-$(VERSION).tar ubqt-$(VERSION)
	@gzip ubqt-$(VERSION).tar
	@rm -rf ubqt-$(VERSION)

clean:
	@echo cleaning
	@rm -rf $(OBJ) *~ ubqt draw/*/*.o

install: all
	@echo installing executable file to $(DESTDIR)$(PREFIX)/bin
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f ubqt $(DESTDIR)$(PREFIX)/bin/ubqt
	@chmod 755 $(DESTDIR)$(PREFIX)/bin/ubqt
	@echo installing manual page to $(DESTDIR)$(MANPREFIX)/man1
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@sed "s/VERSION/$(VERSION)/c" < ubqt.1 > $(DESTDIR)$(MANPREFIX)/man1/ubqt.1
	@chmod 644 $(DESTDIR)$(MANPREFIX)/man1/ubqt.1

uninstall:
	@echo removing executable file from $(DESTDIR)$(PREFIX)/bin
	@rm -f $(DESTDIR)$(PREFIX)/bin/ubqt
	@echo removing manual page from $(DESTDIR)$(MANPREFIX)/man1
	rm -f $(DESTDIR)$(MANPREFIX)/man1/ubqt.1

.PHONY: options clean dist install uninstall
