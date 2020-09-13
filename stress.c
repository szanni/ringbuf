#define _XOPEN_SOURCE 600
#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "ringbuf.h"

struct ringbuf *rb;
uint32_t seed;
#define MSG_MAX_LEN 255

/* Taken from hash-prospector */
static uint32_t
hash32 (uint32_t x)
{
        x ^= x >> 16;
        x *= UINT32_C(0x7feb352d);
        x ^= x >> 15;
        x *= UINT32_C(0x846ca68b);
        x ^= x >> 16;
        return x;
}

/* Simple xor-shift fake random as random() is too slow. */
static uint32_t
fast_rand_32 ()
{
	seed = hash32(seed);
	return seed;
}

/* Return the number of bytes the message contains. */
size_t
msg_gen (uint8_t *buf, size_t buf_size)
{
	size_t i = 0;
	uint8_t cksum = 0;
	size_t size = (buf_size < MSG_MAX_LEN) ? buf_size : MSG_MAX_LEN;
	size = fast_rand_32() % (size-2);

	buf[i] = size;
	while (size > 0) {
		buf[++i] = fast_rand_32();
		cksum ^= buf[i];
		--size;
	}
	buf[++i] = cksum;

	return buf[0] + 2;
}

/* Return the number of bytes to advance in the buffer.
 * Return 0 indicates the message in incomplete. */
size_t
msg_verify (uint8_t *buf, size_t buf_size)
{
	size_t i = 0;
	uint8_t cksum = 0;

	if (buf_size < 2)
		return 0;

	size_t size = buf[i];
	if (buf_size < size + 2)
		return 0;

	while (size > 0) {
		cksum ^= buf[++i];
		--size;
	}
	assert(buf[++i] == cksum);

	return buf[0] + 2;
}

void *
stress_read (void *arg)
{
	size_t num_msgs = *(size_t*)arg;
	uint8_t buf[MSG_MAX_LEN];

	size_t advance = 0;
	size_t read = 0;
	while (num_msgs > 0) {
		read += ringbuf_read(rb, buf+read, MSG_MAX_LEN-read);
		advance = msg_verify(buf, read);
		if (advance != 0)
			--num_msgs;
		read -= advance;
		memmove(buf, buf+advance, read);
		// assert we make progress
		if (read == MSG_MAX_LEN)
			assert(advance != 0);
	}

	return NULL;
}

void *
stress_write (void *arg)
{
	size_t i;
	size_t num_msgs = *(size_t*)arg;
	uint8_t buf[MSG_MAX_LEN];

	for (i = 0; i < num_msgs; ++i) {
		size_t written = 0;
		size_t size = msg_gen(buf, MSG_MAX_LEN);
		while (written < size)
			written += ringbuf_write(rb, buf+written, size-written);
	}

	return NULL;
}

int
main (void)
{
	size_t i;
	size_t size;
	size_t num_msgs;
	pthread_t thread_read;
	pthread_t thread_write;

	seed = random();

	for (i = 1; i < 32; i*=2) {
		size = 1024 * i;
		num_msgs = size * 1024;
		rb = ringbuf_new(size);
		printf("Stress test ringbuf size %zu, number of messages %zu\n", size, num_msgs);

		assert(pthread_create(&thread_write, NULL, stress_write, &num_msgs) == 0);
		assert(pthread_create(&thread_read, NULL, stress_read, &num_msgs) == 0);

		assert(pthread_join(thread_write, NULL) == 0);
		assert(pthread_join(thread_read, NULL) == 0);

		ringbuf_free(rb);
	}
	return 0;
}
