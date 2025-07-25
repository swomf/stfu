PREFIX ?= /usr/local
PROG := stfu

ZSH_COMPLETION_DIR ?= $(DESTDIR)$(PREFIX)/share/zsh/site-functions
BASH_COMPLETION_DIR ?= $(DESTDIR)$(PREFIX)/share/bash-completion/completions

src/$(PROG): src/$(PROG).c src/process.c

install: src/$(PROG)
	@# symlink binary
	install -Dm755 src/$(PROG) $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG)
	ln -rsf $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG) $(DESTDIR)$(PREFIX)/bin/$(PROG)
	@# scripts for running posix/fish
	install -Dm755 src/$(PROG).fish $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG).fish
	install -Dm755 src/$(PROG).sh $(DESTDIR)$(PREFIX)/share/$(PROG)/$(PROG).sh
	@# manpage
	install -Dm644 man/$(PROG).1 $(DESTDIR)$(PREFIX)/share/man/man1/$(PROG).1
	@# completions
	install -Dm644 ./completions/bash/$(PROG) $(BASH_COMPLETION_DIR)/$(PROG)
	install -Dm644 ./completions/zsh/_$(PROG) $(ZSH_COMPLETION_DIR)/_$(PROG)

uninstall:
	@# symlink binary
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(PROG)
	@# share dir
	$(RM) $(DESTDIR)$(PREFIX)/share/$(PROG)/*
	@# completions
	$(RM) $(BASH_COMPLETION_DIR)/_$(PROG)
	$(RM) $(ZSH_COMPLETION_DIR)/_$(PROG)
	@# manpage
	$(RM) $(DESTDIR)$(PREFIX)/share/man/man1/$(PROG).1

clean:
	$(RM) src/$(PROG)

.PHONY: install uninstall clean
