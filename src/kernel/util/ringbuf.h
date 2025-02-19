#ifndef _RINGBUF
#define _RINGBUF

#include "types.h"

struct ringbuf {
	void *buffer; // ptr to data block
	uint32_t cap; // size of block pointed to by buffer
	uint32_t start; // index of starting element
	uint32_t end; // index after ending element
};

// returns number of bytes in the ring buffer
uint32_t ringbuf_len(const struct ringbuf *buf);
// put len bytes from data into the ring buffer. returns false if there isn't enough space
bool ringbuf_put(struct ringbuf *buf, const void *data, uint32_t len);
// read len bytes into out from the ring buffer. returns false if there isn't enough data
bool ringbuf_peek(const struct ringbuf *buf, void *out, uint32_t len);
// take len bytes into out from the ring buffer. returns false if there isn't enough data
bool ringbuf_take(struct ringbuf *buf, void *out, uint32_t len);
// clear all data in the ring buffer
void ringbuf_clear(struct ringbuf *buf);

#endif
