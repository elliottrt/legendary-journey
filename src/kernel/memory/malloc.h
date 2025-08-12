#ifndef _KERNEL_MALLOC
#define _KERNEL_MALLOC

#include "common/types.h"

// malloc block size in pages
#define MALLOC_ALLOC_PAGES (0x100)

#define RMALLOC_DEBUG
#define RMALLOC_NO_ASSERT
#define RMALLOC_NO_STDLIB
#define RMALLOC_NO_STRING
#include "kernel/memory/librmalloc.h"

extern rm_ctx_t *rm_global_ctx;

#endif
