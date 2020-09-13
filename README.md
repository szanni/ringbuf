ringbuf
=======

*ringbuf* is a lock-free, single producer/single consumer (SPSC) ringbuffer
implementation using C11 atomics.

Its aim is to be as simple as possible, while being correct. So no fancy
look ahead functions, memory reserving or similar.

*ringbuf* consists of 4 functions `new`, `read`, `write`, and `free`.

The backing buffer consists of an `uint8_t` array of user specified size.

*ringbuf* is non-blocking for both `read` and `write` operations. `read` and
`write` operations will return the number of bytes read/written.

*ringbuf* does partial `read` and `write` depending on how full/empty the
ringbuffer is and how big the user supplied buffers are.

*ringbuf* was originally developed for audio applications, aiding in the
separation of audio decoding and PCM output.

Usage
=====

To use the single file header library simply include drop `ringbuf.h` into
your source directory and include it:

    #include "ringbuf.h"

Then create a ringbuf instance. The size will be rounded up to the next power
of 2.

    struct ringbuf *rb = ringbuf_new(1024);

Now create your producer and consumer threads. The producer can now write to
the buffer by calling `ringbuf_write`.

    ringbuf_write(rb, buf, buf_size); //returns number of bytes written

Make sure to check the returned size, as it might be smaller than `buf_size`
depending on how full the ringbuffer is.

On the consumer thread you can now read from the ringbuffer by calling
`ringbuf_read`.

    ringbuf_read(rb, buf, buf_size); //returns number of bytes read

This will write a maximum of `buf_size` bytes into the buffer `buf`, depending
on how full the ringbuffer is. Check the return value on how many bytes
were actually read/written.

Once all operations have completed make sure to free the ringbuffer by calling:

    ringbuf_free(rb);

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
