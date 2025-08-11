#include "common/std.h"
#include "kernel/graphics/printf.h"

#define RMALLOC_IMPLEMENTATION
#define RMALLOC_MEMSET memset
#define RMALLOC_MEMCPY memcpy
#include "kernel/memory/malloc.h"
