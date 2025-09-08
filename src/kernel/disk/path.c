#include "kernel/disk/path.h"
#include "common/file.h"

int path_concat(const char *a, const char *b, char *dest, size_t dest_size) {
	if (!a || !b || !dest) {
		errno = EINVAL;
		return -1;
	}
	if (dest_size == 0) {
		return 0;
	}

	int chars_a = path_copy(a, dest, dest_size);
	if (chars_a < 0) return -1;

	// if dest is already full, we can't do anything else
	if ((size_t) chars_a == dest_size) return chars_a;

	// write path separator to join them
	// replacing the null separator written by path_copy
	dest[chars_a - 1] = '/';

	int chars_b = path_copy(b, dest + chars_a, dest_size - chars_a);
	if (chars_b < 0) return -1;

	// null terminate dest
	dest[chars_a + chars_b - 1] = '\0';

	// simplify the result path
	if (path_simplify(dest, dest, dest_size) < 0) return -1;

	return chars_a + chars_b;
}

int path_copy(const char *source, char *dest, size_t dest_size) {
	if (!source || !dest) {
		errno = EINVAL;
		return -1;
	}
	if (dest_size == 0) {
		return 0;
	}

	size_t i = 0;
	for (; source[i] != '\0' && i < dest_size - 1; i++) {
		dest[i] = source[i];
	}
	dest[i] = '\0';

	if (i > PATH_MAX) {
		errno = ENAMETOOLONG;
		return -1;
	} else {
		// include null terminator in count
		return i + 1;
	}
}

int path_simplify(const char *path, char *dest, size_t dest_size) {
	if (dest_size == 0) return 0;

	// make sure dest is null terminated
	dest[dest_size - 1] = '\0';

	size_t path_size = strlen(path);

	if (path_size + 1 >= PATH_MAX) {
		errno = ENAMETOOLONG;
		return -1;
	}

	char *d = dest + dest_size - 2;
	const char *s = path + path_size - 1;

	// remove trailing PATH_SEP unless path = "/"
	if (path_size > 1 && *s == PATH_SEP) {
		s--;
	}

	int dot_dot_count = 0; // for ..
	int should_drop_sep = 0; // for .
	
	for (; s >= path && d >= dest;) {

		// check for path separator
		if (*s == PATH_SEP) {
			if (!should_drop_sep || s == path) *d-- = PATH_SEP;
			else should_drop_sep = 0;

			// don't include repeated sequential separators
			while (s >= path && *s == PATH_SEP) {
				s--;
			}
		}

		// read next path component
		const char *component_end = s + 1;

		// find start of this component
		while (s >= path && *s != PATH_SEP) {
			s--;
		}

		const char *component_start = s + 1;
		size_t component_size = component_end - component_start;
		
		if (component_size == 1 && component_start[0] == '.') {
			should_drop_sep = 1;
		} else if (component_size == 2 && component_start[0] == '.' && component_start[1] == '.') {
			dot_dot_count++;
			should_drop_sep = 1;
		} else {
			// regular component
			if (dot_dot_count > 0) {
				// we have a .. before this so this isn't needed
				dot_dot_count--;
				should_drop_sep = 1;
			} else {
				// copy component onto destination
				for (size_t i = 0; i < component_size && d >= dest; i++) {
					*d-- = component_start[component_size - i - 1];
				}
			}
		}
	}

	// if the path startswith ./ or equals . then we can drop it
	// but if the path starts with '/.' we don't want to drop the leading /
	int preserve_leading_sep = path_size > 0 && path[0] == PATH_SEP && d[2] != PATH_SEP;
	if (should_drop_sep && dot_dot_count == 0 && !preserve_leading_sep) {
		d++;
	}

	// we need to add any ../ at the beginning
	while (dot_dot_count > 0) {
		// make sure we aren't writing outside of dest
		if (d - 3 < dest) {
			errno = ENAMETOOLONG;
			return -1;
		}

		// don't add the PATH_SEP if there's already one there
		// or if we're at the end of dest so we don't add a trailing PATH_SEP

		int at_end = d >= dest + dest_size - 2;

		if (!at_end && d[1] != PATH_SEP) *d-- = PATH_SEP;
		*d-- = '.';
		*d-- = '.';
		dot_dot_count--;
	}

	// because we start working at the end of dest,
	// we need to shift the whole path up to the start of dest.
	// TODO: use memmove?
	size_t shift = d - dest + 1;
	for (size_t i = 0; i < dest_size - shift; i++) {
		dest[i] = dest[i + shift];
	}

	// null terminate dest
	dest[dest_size - shift] = '\0';

	// return chars used. we will always write at least the null terminator
	// so a minimum of 1 char
	int chars_used = dest + dest_size - d - 1;
	return chars_used ? chars_used : 1;
}

int path_exists(const char *path, bool require_dir) {
	errno = 0;
	if (fileopen(NULL, path, FEXISTS | (require_dir ? FDIRECTORY : 0)))
		return 1;
	else if (errno == ENOENT)
		return 0;
	else
		return -1;
}

uint32_t path_next(const char **start) {
	const char *cursor = *start;
	uint32_t size = 0;
	// move past current path
	while (*cursor != PATH_SEP && *cursor) {
		(*start)++;
		cursor++;
	}
	// ignore multiple path sep
	while (*cursor == PATH_SEP) {	
		(*start)++;
		cursor++;
	}
	// *start is where it should be, continue to
	// find the size of the path
	while ( *cursor != PATH_SEP && *cursor) {
		size++;
		cursor++;
	}
	return size;
}

int path_load(const char *parent, const char *child, char *dest, size_t dest_size) {
	if (!parent || !child || !dest) {
		errno = EINVAL;
		return -1;
	}

	// check if absolute path
	if (*child == '/') {
		// load straight into temp_path
		if (path_copy(child, dest, dest_size) < 0) return -1;
		return path_simplify(dest, dest, dest_size);
	} else {
		// relative path, concat with user path
		return path_concat(parent, child, dest, dest_size);
	}
}
