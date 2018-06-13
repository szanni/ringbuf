#include "ringbuf.h"
#include <stdio.h>
#include <assert.h>

void
test_new ()
{
	struct ringbuf *rb = ringbuf_new(8);
	ringbuf_free(rb);
}

void
test_write_fill ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_write(rb, buf, 8) == 8);
	assert(ringbuf_write(rb, buf, 8) == 0);

	ringbuf_free(rb);
}

void
test_write_fill_chunks ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_write(rb, buf, 1) == 1);
	assert(ringbuf_write(rb, buf, 4) == 4);
	assert(ringbuf_write(rb, buf, 2) == 2);
	assert(ringbuf_write(rb, buf, 1) == 1);
	assert(ringbuf_write(rb, buf, 1) == 0);

	ringbuf_free(rb);
}

void
test_write_overfill ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_write(rb, buf, 10) == 8);
	assert(ringbuf_write(rb, buf, 8) == 0);

	ringbuf_free(rb);
}

void
test_write_overfill_chunks ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_write(rb, buf, 1) == 1);
	assert(ringbuf_write(rb, buf, 4) == 4);
	assert(ringbuf_write(rb, buf, 2) == 2);
	assert(ringbuf_write(rb, buf, 7) == 1);
	assert(ringbuf_write(rb, buf, 1) == 0);

	ringbuf_free(rb);
}


void
test_read_empty ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_read(rb, buf, 1) == 0);
	assert(ringbuf_read(rb, buf, 8) == 0);

	ringbuf_free(rb);
}

void
test_overread_empty ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_read(rb, buf, 10) == 0);

	ringbuf_free(rb);
}

void
test_write_full_read_all ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_write(rb, buf, 8) == 8);
	assert(ringbuf_read(rb, buf, 8) == 8);
	assert(ringbuf_read(rb, buf, 2) == 0);

	ringbuf_free(rb);
}

void
test_write_full_read_chunks ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_write(rb, buf, 8) == 8);
	assert(ringbuf_read(rb, buf, 4) == 4);
	assert(ringbuf_read(rb, buf, 2) == 2);
	assert(ringbuf_read(rb, buf, 2) == 2);
	assert(ringbuf_read(rb, buf, 0) == 0);

	ringbuf_free(rb);
}

void
test_write_full_overread ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_write(rb, buf, 8) == 8);
	assert(ringbuf_read(rb, buf, 10) == 8);
	assert(ringbuf_read(rb, buf, 0) == 0);

	ringbuf_free(rb);
}

void
test_write_full_overread_chunks ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_write(rb, buf, 8) == 8);
	assert(ringbuf_read(rb, buf, 4) == 4);
	assert(ringbuf_read(rb, buf, 8) == 4);
	assert(ringbuf_read(rb, buf, 0) == 0);

	ringbuf_free(rb);
}

void
test_write_wrap ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_write(rb, buf, 4) == 4);
	assert(ringbuf_read(rb, buf, 2) == 2);
	assert(ringbuf_write(rb, buf, 6) == 6);
	assert(ringbuf_write(rb, buf, 2) == 0);

	ringbuf_free(rb);
}

void
test_read_wrap ()
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert(rb != NULL);
	assert(ringbuf_write(rb, buf, 4) == 4);
	assert(ringbuf_read(rb, buf, 2) == 2);
	assert(ringbuf_write(rb, buf, 6) == 6);
	assert(ringbuf_read(rb, buf, 8) == 8);
	assert(ringbuf_read(rb, buf, 0) == 0);

	ringbuf_free(rb);
}

int
main(void)
{
	test_new();
	test_write_fill();
	test_write_fill_chunks();
	test_write_overfill();
	test_write_overfill_chunks();
	test_read_empty();
	test_overread_empty();
	test_write_full_read_all();
	test_write_full_read_chunks();
	test_write_full_overread();
	test_write_full_overread_chunks();
	test_write_wrap();
	test_read_wrap();

	puts("[ All tests passed ]");

	return 0;
}

