CC?=clang
CFLAGS+= -std=c11
CFLAGS+= -Wall -Wextra -pedantic
CFLAGS+= `pkg-config --cflags cmocka`
LDFLAGS+= `pkg-config --libs cmocka`

all: test
	@./test

test: test.c ringbuf.h
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f test

.PHONY: all clean
