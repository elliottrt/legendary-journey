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

uint32_t timer_read(void) {
	return 0;
}

FILE *fopen(const char *path, int flags) {
	UNUSED(path);
	UNUSED(flags);

	return NULL;
}

int fclose(FILE *fp) {
	UNUSED(fp);

	return -1;
} 

size_t fread(void *buffer, size_t size, size_t count, FILE *fp) {
	UNUSED(buffer);
	UNUSED(size);
	UNUSED(count);
	UNUSED(fp);

	return 0;
}

size_t fwrite(const void *buffer, size_t size, size_t count, FILE *fp) {
	UNUSED(buffer);
	UNUSED(size);
	UNUSED(count);
	UNUSED(fp);

	return 0;
}

long ftell(FILE *fp) {
	UNUSED(fp);

	return -1;
}

int fseek(FILE *fp, uint32_t offset) {
	UNUSED(fp);
	UNUSED(offset);

	return -1;
}

int fflush(FILE *fp) {
	UNUSED(fp);

	return -1;
}

void frewind(FILE *fp) {
	UNUSED(fp);
}

int fresize(FILE *fp, uint32_t size) {
	UNUSED(fp);
	UNUSED(size);

	return -1;
}

uint32_t fsize(FILE *fp) {
	UNUSED(fp);

	return 0;
}

void *malloc(size_t size) {
	UNUSED(size);

	return NULL;
}

void free(void *ptr) {
	UNUSED(ptr);
}
