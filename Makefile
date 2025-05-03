PREFIX ?= /usr/local
PROG := stfu

stfu: stfu.c

install:
	install -Dm755 $(PROG) $(DESTDIR)$(PREFIX)/bin/$(PROG)

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(PROG)

clean:
	$(RM) $(PROG)

.PHONY: install uninstall clean
