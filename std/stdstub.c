#include "std/std.h"

#define UNUSED(V) ((void) V)

void printf(const char *format, ...) {
	UNUSED(format);
}

void putc(char ch) {
	UNUSED(ch);
}

void puts(const char *str) {
	UNUSED(str);
}

uint64_t timer_read(void) {
	return 0;
}

bool fileopen(struct file *file, const char *pathname, int flags) {
	UNUSED(file);
	UNUSED(pathname);
	UNUSED(flags);

	return false;
}

int32_t fileread(struct file *file, void *buffer, uint32_t size) {
	UNUSED(file);
	UNUSED(buffer);
	UNUSED(size);

	return 0;
}

int32_t filewrite(struct file *file, const void *buffer, uint32_t size) {
	UNUSED(file);
	UNUSED(buffer);
	UNUSED(size);

	return 0;
}

uint32_t filesize(struct file *file) {
	UNUSED(file);

	return 0;
}

bool fileresize(struct file *file, uint32_t size) {
	UNUSED(file);
	UNUSED(size);

	return false;
}

bool fileseek(struct file *file, uint32_t seek) {
	UNUSED(file);
	UNUSED(seek);

	return false;
}

uint32_t filetell(struct file *file) {
	UNUSED(file);

	return 0;
}

bool fileflush(struct file *file) {
	UNUSED(file);

	return false;
}

bool fileclose(struct file *file) {
	UNUSED(file);

	return false;
}
