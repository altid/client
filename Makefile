# ubqt - simple frontend for file server-based programs
# See LICENSE for copyright and license details.

# Have make output usage with no args
options:
	@echo "Usage: make <backend> [<input>]"
	@echo "backends = cairo"
	@echo "         = ncurses"
	@echo "         = nuklear"
	@echo "(input)  = vi"
	@echo "         = game"
	@echo "(data)	= local"
	@echo "         = socket"
	@echo "         = 9p"

# := defines as immediately assigned, for += assignments done later
SRC := src/ubqt.c src/util.c
OBJ := src/ubqt.o src/util.o

include plugin/*/*.mk
include config.mk

include $(SRC:.c=.d)

# dep files
# create temp file with output of -M (deplist)
# insert .d, so they depend on the same things as .o
%.d: %.c
	@set -e; rm -f $@; \
		$(CC) -M $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -rf $@.$$$$

# cc -c -l %.c -o %.o
%.o: %.c %.d
	@echo LD $@
	@$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

# if ubqt.h changes, update all objects
$(OBJ) : src/ubqt.h config.mk

# if any objects change, update ubqt
ubqt: $(OBJ)
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
	@rm -rf $(OBJ) $(OBJ:.o=.d) */*/*.o */*/*.d *~ ubqt

install: ubqt
	@echo installing executable file to $(DESTDIR)$(PREFIX)/bin
	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@cp -f ubqt $(DESTDIR)$(PREFIX)/bin/ubqt
	@chmod 755 $(DESTDIR)$(PREFIX)/bin/ubqt
	@echo installing manual page to $(DESTDIR)$(MANPREFIX)/man1
	@mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	@sed "s/VERSION/${VERSION}/g" < ubqt.1 > $(DESTDIR)$(MANPREFIX)/man1/ubqt.1
	@chmod 644 $(DESTDIR)$(MANPREFIX)/man1/ubqt.1

uninstall:
	@echo removing executable file from $(DESTDIR)$(PREFIX)/bin
	@rm -f $(DESTDIR)$(PREFIX)/bin/ubqt
	@echo removing manual page from $(DESTDIR)$(MANPREFIX)/man1
	rm -f $(DESTDIR)$(MANPREFIX)/man1/ubqt.1

.PHONY: options clean dist install uninstall
