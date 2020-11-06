# See LICENSE for copyright and license details.

SRC := altid.c
OBJ := altid.o

all: altid

include config.mk
include c9/c9.mk
include draw/draw.mk
include mdns/mdns.mk

# cc -c -l %.c -o %.o
%.o: %.c ${INCLUDES}
	@echo LD $@
	@$(CC) -c $(CFLAGS) $< -o $@

# if any objects change, update altid
altid: $(OBJ)
	@echo CC -o $@
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

dist: clean
	@mkdir -p altid-$(VERSION)
	@cp -R LICENSE Makefile README.md altid.1 $(SRC) $(OBJ) altid-$(VERSION)
	@tar -cf altid-$(VERSION).tar altid-$(VERSION)
	@gzip altid-$(VERSION).tar
	@rm -rf altid-$(VERSION)

# from https://stackoverflow.com/a/42908613
# Clean up code from other .mk files
clean:
	@rm -rf $(OBJ) draw/*.gch *~ altid $(CLEAN)

install: altid
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f altid $(DESTDIR)$(PREFIX)/bin/altid
	@chmod 755 $(DESTDIR)$(PREFIX)/bin/altid
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@sed "s/VERSION/${VERSION}/g" < altid.1 > $(DESTDIR)$(MANPREFIX)/man1/altid.1
	@chmod 644 $(DESTDIR)$(MANPREFIX)/man1/altid.1

uninstall:
	@rm -f $(DESTDIR)$(PREFIX)/bin/altid-$(DRAW)
	rm -f $(DESTDIR)$(MANPREFIX)/man1/altid.1

.PHONY: all options clean dist install uninstall
