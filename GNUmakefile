PROJECT=c-uuri
VERSION=1.0.0
PREFIX=/usr/local
PROGS=uuri$(EXE)
TESTS=uuri_test$(EXE)
CC=gcc -Wall -std=c99

all: $(PROGS) $(TESTS)
clean:
	rm -f $(PROGS) $(TESTS)
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(PROGS) $(DESTDIR)$(PREFIX)/bin
	mkdir -p $(DESTDIR)$(PREFIX)/include
	cp uuri.h $(DESTDIR)$(PREFIX)/include

uuri$(EXE): main.c uuri.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ main.c
uuri_test$(EXE): test.c uuri.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ test.c




