#include <stdlib.h>

#define UNUSED(V) ((void) V)

// entry point of program, otherwise need a linker flag
int _start(int argc, char **argv) {
	extern int main(int, char**);
	return main(argc, argv);
}

extern const char *getcwd(void) {
	return NULL;
}

extern int setcwd(const char *path) {
	UNUSED(path);
	return -1;
}

int printf(const char *format, ...) {
	UNUSED(format);
	return -1;
}

int putchar(int ch) {
	UNUSED(ch);
	return -1;
}

int puts(const char *str) {
	UNUSED(str);
	return -1;
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
