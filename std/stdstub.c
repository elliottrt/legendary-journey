#include "std/std.h"

void printf(const char *format, ...) {

}

void putc(char ch) {

}

void puts(const char *str) {

}

uint64_t timer_read(void) {
	return 0;
}

bool fileopen(struct file *file, const char *pathname, int flags) {
	return false;
}

int32_t fileread(struct file *file, void *buffer, uint32_t size) {
	return 0;
}

int32_t filewrite(struct file *file, const void *buffer, uint32_t size) {
	return 0;
}

uint32_t filesize(struct file *file) {
	return 0;
}

bool fileresize(struct file *file, uint32_t size) {
	return false;
}

bool fileseek(struct file *file, uint32_t seek) {
	return false;
}

uint32_t filetell(struct file *file) {
	return 0;
}

bool fileflush(struct file *file) {
	return false;
}

bool fileclose(struct file *file) {
	return false;
}
