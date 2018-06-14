#ifndef RINGBUF_H
#define RINGBUF_H

#include <stdlib.h>
#include <stdatomic.h>
#include <errno.h>
#include <string.h>

struct ringbuf {
	_Atomic size_t read_idx;
	_Atomic size_t write_idx;
	size_t capacity;
	uint8_t data[1];
};

/*!
 * \brief Allocate a new SPSC ring buffer.
 *
 * Allocate a new single producer, single consumer ring buffer.
 * The capacity will be rounded up to the next power of 2.
 *
 * \param capacity Capacity of the ring buffer.
 * \return A pointer to a newly allocated ring buffer, NULL on error.
 */
static struct ringbuf *
ringbuf_new(size_t capacity)
{
	size_t power_of_two;
	struct ringbuf *rb;

	for (power_of_two = 1; 1 << power_of_two < capacity; ++power_of_two) {}

	capacity = 1 << power_of_two;

	rb = malloc(sizeof(rb) + sizeof(rb->data) * (capacity - 1));
	if (rb == NULL)
		return NULL;

	atomic_init(&rb->read_idx, 0);
	atomic_init(&rb->write_idx, capacity * 2);
	rb->capacity = capacity;
	return rb;
}

static inline size_t
_ringbuf_size(struct ringbuf *rb, size_t read_idx, size_t write_idx)
{
	return (write_idx - read_idx) % (rb->capacity * 2);
}

static inline size_t
_ringbuf_min(size_t a, size_t b)
{
	if (a < b)
		return a;
	else
		return b;
}

/*!
 * \brief Write to ring buffer.
 * \warning Only call this function from a single producer thread.
 *
 * \param rb Ring buffer instance.
 * \param buf Buffer holding data to be written to ring buffer.
 * \param buf_size Buffer size in bytes.
 * \return Number of bytes written to ring buffer.
 */
static size_t
ringbuf_write(struct ringbuf *rb, uint8_t *buf, size_t buf_size)
{
	size_t write_idx = atomic_load_explicit(&rb->write_idx, memory_order_relaxed);
	size_t read_idx = atomic_load_explicit(&rb->read_idx, memory_order_acquire);
	size_t size = _ringbuf_size(rb, read_idx, write_idx);
	size_t true_write_idx = write_idx % rb->capacity;

	if (size < rb->capacity) {
		size_t write0, write1;
		size_t write = _ringbuf_min(rb->capacity - size, buf_size);
		size_t write_overflow = true_write_idx + write;
		if (write_overflow > rb->capacity) {
			write1 = write_overflow % rb->capacity;
			write0 = write - write1;
		}
		else {
			write0 = write;
			write1 = 0;
		}

		memcpy(&rb->data[true_write_idx], buf, write0);

		if (write1)
			memcpy(rb->data, buf + write0, write1);

		write_idx = (write_idx + write < rb->capacity * 4) ? write_idx + write : rb->capacity * 2 + write1;
		atomic_store_explicit(&rb->write_idx, write_idx, memory_order_release);
		return write;
	}
	errno = EWOULDBLOCK;
	return 0;
}

/*!
 * \brief Read from ring buffer.
 * \warning Only call this function from a single consumer thread.
 *
 * \param rb Ring buffer instance.
 * \param buf Buffer to copy data to from ring buffer.
 * \param buf_size Buffer size in bytes.
 * \return Number of bytes read from ring buffer.
 */
static size_t
ringbuf_read(struct ringbuf *rb, uint8_t *buf, size_t buf_size)
{
	size_t read_idx = atomic_load_explicit(&rb->read_idx, memory_order_relaxed);
	size_t write_idx = atomic_load_explicit(&rb->write_idx, memory_order_acquire);
	size_t size = _ringbuf_size(rb, read_idx, write_idx);
	size_t true_read_idx = read_idx % rb->capacity;

	if (size > 0)
	{
		size_t read0, read1;
		size_t read = _ringbuf_min(size, buf_size);
		size_t read_overflow = true_read_idx + read;
		if (read_overflow > rb->capacity) {
			read1 = read_overflow % rb->capacity;
			read0 = read - read1;
		}
		else {
			read0 = read;
			read1 = 0;
		}

		memcpy(buf, &rb->data[true_read_idx], read0);

		if (read1)
			memcpy(buf + read0, rb->data, read1);

		read_idx = (read_idx + read < rb->capacity * 2) ? read_idx + read : read1;
		atomic_store_explicit(&rb->read_idx, read_idx, memory_order_release);
		return read;
	}
	errno = EWOULDBLOCK;
	return 0;
}

/*!
 * \brief Free allocated ring buffer.
 * \param rb Pointer to ring buffer or NULL.
 */
static void
ringbuf_free(struct ringbuf *rb)
{
	free(rb);
}


#endif

