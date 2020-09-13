ringbuf
=======

*ringbuf* is a lock-free, single producer/single consumer (SPSC) ring
buffer/circular buffer implementation using C11 atomics.

Its aim is to be as simple as possible, while being correct. So no fancy
look ahead functions, memory reserving or similar.

*ringbuf* consists of 4 functions `new`, `read`, `write`, and `free`.

The backing buffer consists of an `uint8_t` array of user specified size. All
`read` and `write` operations copy either from or to this buffer.

*ringbuf* is non-blocking for both `read` and `write` operations. `read` and
`write` operations will return the number of bytes read/written.

*ringbuf* does partial `read` and `write` depending on how full/empty the
ring buffer is and how big the user supplied buffers are.

*ringbuf* was originally developed for audio applications, aiding in the
separation of audio decoding and PCM output.

Usage
=====

To use the single file header library simply drop `ringbuf.h` into your source
directory and include it.

```c
#include "ringbuf.h"
```

Then create a *ringbuf* instance. The size will be rounded up to the next power
of 2.

```c
struct ringbuf * ringbuf_new(size_t capacity);
```

Now create your producer and consumer threads. The producer can now write to
the buffer by calling `ringbuf_write`.

```c
size_t ringbuf_write(struct ringbuf *rb, uint8_t *buf, size_t buf_size);
```

Make sure to check the returned size, as it might be smaller than `buf_size`
depending on how full the ring buffer is.

On the consumer thread you can now read from the ring buffer by calling
`ringbuf_read`.

```c
size_t ringbuf_read(struct ringbuf *rb, uint8_t *buf, size_t buf_size);
```

This will read a maximum of `buf_size` bytes into the buffer `buf`, depending
on how full the ring buffer is. Check the return value on how many bytes
were actually copied.

Once all operations have completed make sure to free the ring buffer by calling:

```c
void ringbuf_free(struct ringbuf *rb);
```

Dependencies
============

Runtime
-------

* A C11 compatible compiler with C11 atomics support

Test
----

* Cmocka

License
=======

ISC license, see the LICENSE file for more details.
