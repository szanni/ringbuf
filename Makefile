.PHONY: all check stress install dist clean

INSTALL ?= install
PREFIX ?= /usr/local
DESTDIR ?=
CC ?= cc
PKG_CONFIG ?= pkg-config
CFLAGS += -std=c11
CFLAGS += -Wall -Wextra -pedantic
CFLAGS += -pthread
#CFLAGS += -fsanitize=address
#CFLAGS += -fno-omit-frame-pointer
CFLAGS += -fsanitize=undefined
CFLAGS += -fsanitize=thread
CFLAGS += -fprofile-arcs -ftest-coverage
CFLAGS += -O3
CFLAGS += `$(PKG_CONFIG) --cflags cmocka`
LDFLAGS += `$(PKG_CONFIG) --libs cmocka`

PACKAGE = ringbuf
VERSION = 0.0.0

DIST = Makefile ringbuf.h unused.h test.c LICENSE

all:
	@echo 'As this is a header only library there is nothing to be done.'
	@echo 'See `make check` and `make install`.'

check: test
	./test

stress: stress.c ringbuf.h
	$(CC) stress.c -o $@ $(CFLAGS) $(LDFLAGS)
	./stress

test: test.c ringbuf.h unused.h
	$(CC) test.c -o $@ $(CFLAGS) $(LDFLAGS)

install: ringbuf.h
	$(INSTALL) -D -m644 ringbuf.h "$(DESTDIR)$(PREFIX)/include/ringbuf.h"

dist: $(DIST)
	mkdir -p $(PACKAGE)-$(VERSION)
	cp $(DIST) $(PACKAGE)-$(VERSION)
	tar -cf $(PACKAGE)-$(VERSION).tar $(PACKAGE)-$(VERSION)
	gzip -fk $(PACKAGE)-$(VERSION).tar
	xz -f $(PACKAGE)-$(VERSION).tar

clean:
	rm -f test stress *.gcov *.gcda *.gcno

distclean: clean
	rm -rf $(PACKAGE)-$(VERSION){,.tar.gz,.tar.xz}
