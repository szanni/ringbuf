CC?=clang
CFLAGS+= -std=c11 -Wall -Wextra -pedantic

all: test
	@./test

test: test.c ringbuf.h
	$(CC) $(CFLAGS) $< -o $@

.PHONY: all
