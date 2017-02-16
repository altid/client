# ubqt - simple frontend for file server-based programs
# See LICENSE for copyright and license details.


SRC := src/ubqt.c src/util.c
OBJ := src/ubqt.o src/util.o

include config.mk
include plugins/$(TRANSPORT)/$(TRANSPORT).mk
include plugins/$(BACKEND)/$(BACKEND).mk
include plugins/$(SEAT)/$(SEAT).mk

all: ubqt-$(BACKEND)

# cc -c -l %.c -o %.o
%.o: %.c
	@echo LD $@
	@$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# if ubqt.h changes, update all objects
$(OBJ) : src/ubqt.h

# if any objects change, update ubqt
ubqt-$(BACK): $(OBJ)
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
	@rm -rf $(OBJ) *~ ubqt-$(BACKEND)

install: ubqt-$(BACKEND)
	@echo installing executable file to $(DESTDIR)$(PREFIX)/bin
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f ubqt-$(BACKEND) $(DESTDIR)$(PREFIX)/bin/ubqt-$(BACKEND)
	@chmod 755 $(DESTDIR)$(PREFIX)/bin/ubqt-$(BACKEND)
	@echo installing manual page to $(DESTDIR)$(MANPREFIX)/man1
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@sed "s/VERSION/${VERSION}/g" < ubqt.1 > $(DESTDIR)$(MANPREFIX)/man1/ubqt.1
	@chmod 644 $(DESTDIR)$(MANPREFIX)/man1/ubqt.1

uninstall:
	@echo removing executable file from $(DESTDIR)$(PREFIX)/bin
	@rm -f $(DESTDIR)$(PREFIX)/bin/ubqt-$(BACKEND)
	@echo removing manual page from $(DESTDIR)$(MANPREFIX)/man1
	rm -f $(DESTDIR)$(MANPREFIX)/man1/ubqt.1

.PHONY: all options clean dist install uninstall
