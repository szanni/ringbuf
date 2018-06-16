#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "unused.h"
#include "ringbuf.h"

void
test_new (void ** UNUSED(state))
{
	struct ringbuf *rb = ringbuf_new(8);
	assert_non_null(rb);
	ringbuf_free(rb);
}

void
test_new_power_of_two_no_increase (void ** UNUSED(state))
{
	struct ringbuf *rb = ringbuf_new(8);
	assert_non_null(rb);
	assert_int_equal(rb->capacity, 8);
	ringbuf_free(rb);
}

void
test_new_non_power_of_two (void ** UNUSED(state))
{
	struct ringbuf *rb = ringbuf_new(7);
	assert_non_null(rb);
	assert_int_equal(rb->capacity, 8);
	ringbuf_free(rb);
}

void
test_write_fill (void ** UNUSED(state))
{
	uint8_t buf[8] = {0};
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_write(rb, buf, 8), 8);
	assert_int_equal(ringbuf_write(rb, buf, 8), 0);

	ringbuf_free(rb);
}

void
test_write_fill_chunks (void ** UNUSED(state))
{
	uint8_t buf[8] = {0};
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_write(rb, buf, 1), 1);
	assert_int_equal(ringbuf_write(rb, buf, 4), 4);
	assert_int_equal(ringbuf_write(rb, buf, 2), 2);
	assert_int_equal(ringbuf_write(rb, buf, 1), 1);
	assert_int_equal(ringbuf_write(rb, buf, 1), 0);

	ringbuf_free(rb);
}

void
test_write_overfill (void ** UNUSED(state))
{
	uint8_t buf[8] = {0};
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_write(rb, buf, 10), 8);
	assert_int_equal(ringbuf_write(rb, buf, 8), 0);

	ringbuf_free(rb);
}

void
test_write_overfill_chunks (void ** UNUSED(state))
{
	uint8_t buf[8] = {0};
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_write(rb, buf, 1), 1);
	assert_int_equal(ringbuf_write(rb, buf, 4), 4);
	assert_int_equal(ringbuf_write(rb, buf, 2), 2);
	assert_int_equal(ringbuf_write(rb, buf, 7), 1);
	assert_int_equal(ringbuf_write(rb, buf, 1), 0);

	ringbuf_free(rb);
}


void
test_read_empty (void ** UNUSED(state))
{
	uint8_t buf[8] = {0};
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_read(rb, buf, 1), 0);
	assert_int_equal(ringbuf_read(rb, buf, 8), 0);

	ringbuf_free(rb);
}

void
test_overread_empty (void ** UNUSED(state))
{
	uint8_t buf[8] = {0};
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_read(rb, buf, 10), 0);

	ringbuf_free(rb);
}

void
test_write_full_read_all (void ** UNUSED(state))
{
	uint8_t src[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	uint8_t dst[8] = {0};
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_write(rb, src, 8), 8);
	assert_int_equal(ringbuf_read(rb, dst, 8), 8);
	assert_int_equal(ringbuf_read(rb, dst, 2), 0);
	assert_memory_equal(src, dst, 8);

	ringbuf_free(rb);
}

void
test_write_full_read_chunks (void ** UNUSED(state))
{
	uint8_t src[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	uint8_t dst[8] = {0};
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_write(rb, src, 8), 8);
	assert_int_equal(ringbuf_read(rb, dst, 4), 4);
	assert_int_equal(ringbuf_read(rb, dst+4, 2), 2);
	assert_int_equal(ringbuf_read(rb, dst+6, 2), 2);
	assert_int_equal(ringbuf_read(rb, dst, 0), 0);
	assert_memory_equal(src, dst, 8);

	ringbuf_free(rb);
}

void
test_write_full_overread (void ** UNUSED(state))
{
	uint8_t src[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	uint8_t dst[8] = {0};
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_write(rb, src, 8), 8);
	assert_int_equal(ringbuf_read(rb, dst, 10), 8);
	assert_int_equal(ringbuf_read(rb, dst, 0), 0);
	assert_memory_equal(src, dst, 8);

	ringbuf_free(rb);
}

void
test_write_full_overread_chunks (void ** UNUSED(state))
{
	uint8_t src[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	uint8_t dst[8] = {0};
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_write(rb, src, 8), 8);
	assert_int_equal(ringbuf_read(rb, dst, 4), 4);
	assert_int_equal(ringbuf_read(rb, dst+4, 8), 4);
	assert_int_equal(ringbuf_read(rb, dst, 0), 0);
	assert_memory_equal(src, dst, 8);

	ringbuf_free(rb);
}

void
test_write_wrap (void ** UNUSED(state))
{
	uint8_t buf[8];
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_write(rb, buf, 4), 4);
	assert_int_equal(ringbuf_read(rb, buf, 2), 2);
	assert_int_equal(ringbuf_write(rb, buf, 6), 6);
	assert_int_equal(ringbuf_write(rb, buf, 2), 0);

	ringbuf_free(rb);
}

void
test_read_wrap (void ** UNUSED(state))
{
	uint8_t src[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	uint8_t dst[8] = {0};
	uint8_t exp[8] = {2, 3, 4, 5, 6, 7, 8, 9};
	struct ringbuf *rb = ringbuf_new(8);

	assert_non_null(rb);
	assert_int_equal(ringbuf_write(rb, src, 4), 4);
	assert_int_equal(ringbuf_read(rb, dst, 2), 2);
	assert_int_equal(ringbuf_write(rb, src+4, 6), 6);
	assert_int_equal(ringbuf_read(rb, dst, 8), 8);
	assert_int_equal(ringbuf_read(rb, dst, 0), 0);
	assert_memory_equal(exp, dst, 8);

	ringbuf_free(rb);
}

int
main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_new),
		cmocka_unit_test(test_new_power_of_two_no_increase),
		cmocka_unit_test(test_new_non_power_of_two),
		cmocka_unit_test(test_write_fill),
		cmocka_unit_test(test_write_fill_chunks),
		cmocka_unit_test(test_write_overfill),
		cmocka_unit_test(test_write_overfill_chunks),
		cmocka_unit_test(test_read_empty),
		cmocka_unit_test(test_overread_empty),
		cmocka_unit_test(test_write_full_read_all),
		cmocka_unit_test(test_write_full_read_chunks),
		cmocka_unit_test(test_write_full_overread),
		cmocka_unit_test(test_write_full_overread_chunks),
		cmocka_unit_test(test_write_wrap),
		cmocka_unit_test(test_read_wrap),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}

