# See LICENSE for copyright and license details.


SRC := src/ubqt.c src/util.c
OBJ := src/ubqt.o src/util.o

include config.mk
include plugins/$(TRANSPORT)/$(TRANSPORT).mk
include plugins/$(DRAW)/$(DRAW).mk
include plugins/$(SEAT)/$(SEAT).mk

all: ubqt-$(DRAW)

# cc -c -l %.c -o %.o
%.o: %.c
	@echo LD $@
	@$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# if ubqt.h changes, update all objects
$(OBJ) : src/ubqt.h

# if any objects change, update ubqt
ubqt-$(DRAW): $(OBJ)
	@echo CC -o $@
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

dist: clean
	@mkdir -p ubqt-$(VERSION)
	@cp -R LICENSE Makefile README.md ubqt.1 $(SRC) $(OBJ) ubqt-$(VERSION)
	@tar -cf ubqt-$(VERSION).tar ubqt-$(VERSION)
	@gzip ubqt-$(VERSION).tar
	@rm -rf ubqt-$(VERSION)

# from https://stackoverflow.com/a/42908613
# Clean up code from other .mk files
clean:
	@rm -rf $(OBJ) *~ ubqt-$(DRAW) $(CLEAN)

install: ubqt-$(DRAW)
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f ubqt-$(DRAW) $(DESTDIR)$(PREFIX)/bin/ubqt-$(DRAW)
	@chmod 755 $(DESTDIR)$(PREFIX)/bin/ubqt-$(DRAW)
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@sed "s/VERSION/${VERSION}/g" < ubqt.1 > $(DESTDIR)$(MANPREFIX)/man1/ubqt.1
	@chmod 644 $(DESTDIR)$(MANPREFIX)/man1/ubqt.1

uninstall:
	@rm -f $(DESTDIR)$(PREFIX)/bin/ubqt-$(DRAW)
	rm -f $(DESTDIR)$(MANPREFIX)/man1/ubqt.1

.PHONY: all options clean dist install uninstall
