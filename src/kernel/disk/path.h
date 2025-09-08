#ifndef _KERNEL_FILE_PATH
#define _KERNEL_FILE_PATH

#include "common/std.h"

// maximum size of a file path including null terminator
#define PATH_MAX (256)

// concatenates two paths and stores the result in dest
// returns:
// 		number of chars in dest on success including null terminator,
// 		<0 on failure (sets errno)
int path_concat(const char *a, const char *b, char *dest, size_t dest_size);

// copies up to dest_size-1 chars from source to dest and null terminates dest
// returns:
//		number of chars copied on success including null terminator,
//		<0 on failure (sets errno)
int path_copy(const char *source, char *dest, size_t dest_size);

// simplifies a path and places the result in dest
// it may or may not be save to have path == dest
// returns:
//		size of the path in dest including null terminator on success,
//		<0 on failure (sets errno)
int path_simplify(const char *path, char *dest, size_t dest_size);

// checks whether the given path exists. this path is always
// considered to be absolute.
// if require_dir is set, the function will check that a directory exists at that path.
// if it is not a directory, returns <0
// returns:
// 		>0 if the path exists,
// 		=0 if the path does not exist,
//		<0 if there was an error, sets errno
int path_exists(const char *path, bool require_dir);

// updates *start, which points to the path,
// to point to the next path component left to right
// returns the size of the current path component
uint32_t path_next(const char **start);

// if child is absolute: copied child to dest
// else: concatenates parent and child into dest
// returns:
//		>=0 on success
//		<0 on failure
int path_load(const char *parent, const char *child, char *dest, size_t dest_size);

#endif
