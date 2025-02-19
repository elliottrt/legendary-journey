#include "util/ringbuf.h"
#include "std.h"

uint32_t ringbuf_len(const struct ringbuf *buf) {
	return buf->end >= buf->start ? buf->end - buf->start : buf->cap - (buf->start - buf->end);
}

uint32_t ringbuf_space(const struct ringbuf *buf) {
	return buf->end >= buf->start ? buf->cap - (buf->end - buf->start) : buf->start - buf->end;
}

bool ringbuf_put(struct ringbuf *buf, const void *data, uint32_t len) {
	// if not enough space, fail
	if (ringbuf_space(buf) < len)
		return false;

	// no point writing
	if (len == 0) return true;

	/*
	end ahead of start -> cap
	end equal to start -> full  -> UNREACHABLE
	end equal to start -> empty -> cap
	end behind   start -> start
	*/

	// put bytes following end and update end
	uint32_t upper_limit = buf->end >= buf->start ? buf->cap : buf->start;
	uint32_t bytes_to_write = umin(upper_limit - buf->end, len);

	// printf("writing %u to %u (%u bytes)\n", buf->end, buf->end + bytes_to_write, bytes_to_write);

	memcpy(buf->buffer + buf->end, data, bytes_to_write);
	buf->end += bytes_to_write;
	data += bytes_to_write;
	len -= bytes_to_write;

	// if we need to wrap around from the beginning
	if (len > 0) {
		// printf("\twriting %u to %u (another %u bytes)\n", 0, len, len);
		memcpy(buf->buffer, data, len);
		buf->end = len;
	}

	// printf("\tend -> %u\n", buf->end);

	return true;
}

bool ringbuf_peek(const struct ringbuf *buf, void *out, uint32_t len) {
	// if not enough to read, fail
	if (ringbuf_len(buf) < len) return false;

	// no point reading
	if (len == 0) return true;

	/*
	end ahead of start -> end
	end equal to start -> empty -> UNREACHABLE
	end equal to start -> full  -> cap
	end behind   start -> cap
	*/

	uint32_t upper_limit = buf->end > buf->start ? buf->end : buf->cap;
	uint32_t bytes_to_read = umin(upper_limit - buf->start, len);

	// printf("reading %u to %u (%u bytes)\n", buf->start, buf->start + bytes_to_read, bytes_to_read);

	memcpy(out, buf->buffer + buf->start, bytes_to_read);
	out += bytes_to_read;
	len -= bytes_to_read;

	// if we need to wrap around from the beginning
	if (len > 0) {
		// printf("\treading %u to %u (another %u bytes)\n", 0, len, len);
		memcpy(out, buf->buffer, len);
	}

	return true;
}

bool ringbuf_take(struct ringbuf *buf, void *out, uint32_t len) {
	uint32_t bytes_left = ringbuf_len(buf);

	// if not enough to read, fail
	if (bytes_left < len) return false;

	// no point reading
	if (len == 0) return true;

	/*
	end ahead of start -> end
	end equal to start -> empty -> UNREACHABLE
	end equal to start -> full  -> cap
	end behind   start -> cap
	*/

	uint32_t upper_limit = buf->end > buf->start ? buf->end : buf->cap;
	uint32_t bytes_to_read = umin(upper_limit - buf->start, len);

	// printf("taking %u to %u (%u bytes)\n", buf->start, buf->start + bytes_to_read, bytes_to_read);

	memcpy(out, buf->buffer + buf->start, bytes_to_read);
	buf->start += bytes_to_read;
	out += bytes_to_read;
	len -= bytes_to_read;

	// if we need to wrap around from the beginning
	if (len > 0) {
		// printf("\ttaking %u to %u (another %u bytes)\n", 0, len, len);
		memcpy(out, buf->buffer, len);
		buf->start = len;
	}

	// printf("\tstart -> %u\n", buf->start);

	return true;
}

void ringbuf_clear(struct ringbuf *buf) {
	buf->start = 0;
	buf->end = 0;
}
