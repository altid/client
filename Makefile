# See LICENSE for copyright and license details.


SRC := src/altid.c src/util.c
OBJ := src/altid.o src/util.o

include config.mk
include plugins/$(TRANSPORT)/$(TRANSPORT).mk
include plugins/$(DRAW)/$(DRAW).mk
include plugins/$(SEAT)/$(SEAT).mk

all: altid-$(DRAW)

# cc -c -l %.c -o %.o
%.o: %.c
	@echo LD $@
	@$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# if altid.h changes, update all objects
$(OBJ) : src/altid.h

# if any objects change, update altid
altid-$(DRAW): $(OBJ)
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
	@rm -rf $(OBJ) *~ altid-$(DRAW) $(CLEAN)

install: altid-$(DRAW)
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f altid-$(DRAW) $(DESTDIR)$(PREFIX)/bin/altid-$(DRAW)
	@chmod 755 $(DESTDIR)$(PREFIX)/bin/altid-$(DRAW)
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@sed "s/VERSION/${VERSION}/g" < altid.1 > $(DESTDIR)$(MANPREFIX)/man1/altid.1
	@chmod 644 $(DESTDIR)$(MANPREFIX)/man1/altid.1

uninstall:
	@rm -f $(DESTDIR)$(PREFIX)/bin/altid-$(DRAW)
	rm -f $(DESTDIR)$(MANPREFIX)/man1/altid.1

.PHONY: all options clean dist install uninstall
