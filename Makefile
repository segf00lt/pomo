CFLAGS = -g -Wall -Wpedantic

PREFIX = /usr/local

all:
	$(CC) $(CFLAGS) pomo.c -o pomo -lraylib

clean:
	rm -f pomo

install: all
	cp -f pomo $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/pomo
	mkdir -p $(PREFIX)/share/pomo
	cp -f bell.wav $(PREFIX)/share/pomo
	chmod u+r $(PREFIX)/share/pomo/bell.wav
	cp -f click.wav $(PREFIX)/share/pomo
	chmod u+r $(PREFIX)/share/pomo/click.wav

uninstall:
	rm -f $(PREFIX)/bin/pomo
	rm -fdr $(PREFIX)/share/pomo

.PHONY: all clean install uninstall
