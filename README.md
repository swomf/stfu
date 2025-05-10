# stfu

shut the filestream up

## what is this?

suppose I want to run firefox right from the terminal but _only_ because it's convenient.
not for any debug log purposes or anything like that.

I often end up doing

```bash
firefox &>/dev/null &
disown
```

now, I can make this a function in my rc file, but I frequently interchange between bash, fish, zsh.

so why not make this a shell-agnostic C program?

## features

* detach and shut up a command from your terminal
* detach and shut up an alias from your terminal (`-s`)

see `man 1 stfu` for more

## installation

```
sudo make install
```

you can set the PREFIX and DESTDIR environment variables as needed.
