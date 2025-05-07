PREFIX ?= /usr/local
PROG := stfu
ZSH_COMPLETION_DIR ?= $(DESTDIR)$(PREFIX)/share/zsh/site-functions
BASH_COMPLETION_DIR ?= $(DESTDIR)$(PREFIX)/share/bash-completion/completions

src/stfu: src/stfu.c src/process.c

install:
	install -Dm755 src/$(PROG) $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG)
	ln -rsf $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG) $(DESTDIR)$(PREFIX)/bin/$(PROG)
	install -Dm755 src/$(PROG).fish $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG).fish
	install -Dm755 src/$(PROG).sh $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG).sh
	install -Dm644 ./completions/zsh/_$(PROG) $(ZSH_COMPLETION_DIR)/_$(PROG)
	install -Dm644 ./completions/bash/$(PROG) $(BASH_COMPLETION_DIR)/$(PROG)

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(PROG)
	$(RM) $(ZSH_COMPLETION_DIR)/_stfu

clean:
	$(RM) $(PROG)

.PHONY: install uninstall clean
