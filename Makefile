PREFIX ?= /usr/local
PROG := stfu
ZSH_COMPLETION_DIR ?= $(DESTDIR)$(PREFIX)/share/zsh/site-functions

stfu: stfu.c

install:
	install -Dm755 $(PROG) $(DESTDIR)$(PREFIX)/bin/$(PROG)
	install -Dm644 ./completions/zsh/_stfu $(ZSH_COMPLETION_DIR)

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(PROG)

clean:
	$(RM) $(PROG)

.PHONY: install uninstall clean
