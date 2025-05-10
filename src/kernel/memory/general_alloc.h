#ifndef _KERNEL_GENERAL_ALLOC
#define _KERNEL_GENERAL_ALLOC

#include "common/types.h"

#define GEN_ALLOC_DEBUG

// size of the allocation block in pages
#define GEN_ALLOC_SIZE (0x1000)

void gen_alloc_init(void *ptr, size_t size);

void *gen_malloc(size_t size);
void gen_free(void *ptr);

void *gen_calloc(size_t count, size_t size);
void *gen_realloc(void *ptr, size_t new_size);

#ifdef GEN_ALLOC_DEBUG
void gen_debug(void);
#endif

#endif // _KERNEL_GENERAL_ALLOC
