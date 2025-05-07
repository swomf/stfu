PREFIX ?= /usr/local
PROG := stfu
ZSH_COMPLETION_DIR ?= $(DESTDIR)$(PREFIX)/share/zsh/site-functions

stfu: stfu.c process.c

install:
	install -Dm755 $(PROG) $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG)
	ln -rsf $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG) $(DESTDIR)$(PREFIX)/bin/$(PROG)
	install -Dm755 $(PROG).fish $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG).fish
	install -Dm755 $(PROG).sh $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG).sh
	install -Dm644 ./completions/zsh/_stfu $(ZSH_COMPLETION_DIR)/_stfu

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(PROG)
	$(RM) $(ZSH_COMPLETION_DIR)/_stfu

clean:
	$(RM) $(PROG)

.PHONY: install uninstall clean
