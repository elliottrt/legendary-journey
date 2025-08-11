// rmalloc.h
// basic context/arena-based memory allocator
// options
//	RMALLOC_DEBUG - enable debug printout, requires stdio.h
//	RMALLOC_NO_ASSERT - disable assertion checks
//	RMALLOC_NO_STDLIB - don't use the stdlib.h
//	RMALLOC_NO_STRING - don't use string.h (can provide own string functions)

#ifndef RMALLOC_H
#define RMALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * DEFINITIONS
*/

// TODO: tests?
// TODO: option for nostdlib that removes LIBC_MALLOC alloc_type

typedef struct rm_context rm_ctx_t;

/*
 * FUNCTION PROTOTYPES
*/

// initialize the memory allocator with a block to manage
rm_ctx_t *rm_init_block(void *ptr, size_t size);

#ifndef RMALLOC_NO_STDLIB
// initialize the memory allocator with a size to manage
// returns NULL if allocation failed
rm_ctx_t *rm_init_malloc(size_t size);
#endif // RMALLOC_NO_STDLIB

// deinitializes the given context based on its allocation method.
// m_init_block:
//		returns the pointer used by the allocator
// 		if the size pointer is not NULL, fills it with the block size
// m_init_malloc:
// 		returns NULL and size is not modified
void *rm_deinit(rm_ctx_t *ctx, size_t *size);

// allocate 'size' bytes of memory in the given context
// and return a pointer to the beginning of the memory
// returns NULL on failure
void *rm_alloc(rm_ctx_t *ctx, size_t size);

// allocate 'count' elements of 'size' bytes each in the given context
// and return a pointer to the beginning of the memory
// which will be initialized with zeroes.
// returns NULL on failure
void *rm_calloc(rm_ctx_t *ctx, size_t count, size_t size);

// resize an allocation in the given context to a new size
// if ptr == NULL, allocates a new block with the given size
// returns NULL on failure
void *rm_realloc(rm_ctx_t *ctx, void *ptr, size_t new_size);

// frees a pointer in the given context
void rm_free(rm_ctx_t *ctx, void *ptr);

#ifdef RMALLOC_DEBUG

// print out a memory map of the allocations within a context
void rm_debug(rm_ctx_t *ctx);

#endif // RMALLOC_DEBUG

/*
 * IMPLEMENTATION
*/

#ifdef RMALLOC_IMPLEMENTATION

#ifndef RMALLOC_NO_ASSERT
	#include <assert.h>
#else // RMALLOC_ASSERT
	#define assert(...)
#endif // RMALLOC_ASSERT

#ifndef RMALLOC_NO_STDLIB
	#include <stdlib.h>
#endif // RMALLOC_NO_STDLIB

#ifndef RMALLOC_NO_STRING
	#include <string.h>
	#define RMALLOC_MEMSET memset
	#define RMALLOC_MEMCPY memcpy
#else // RMALLOC_NO_STRING
	#ifndef RMALLOC_MEMCPY
		#error RMALLOC_NO_STRING was set and RMALLOC_MEMCPY was not defined
	#endif // !RMALLOC_MEMCPY
	#ifndef RMALLOC_MEMSET
		#error RMALLOC_NO_STRING was set and RMALLOC_MEMSET was not defined
	#endif // !RMALLOC_MEMSET
#endif // RMALLOC_NO_STRING

#define GET_BLOCK(P) (((rm_block_t *) (P)) - 1)

#define ALIGNMENT (sizeof(void*))
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

enum rm_alloc_type {
	MAT_USER_PROVIDED = 0,

#ifndef RMALLOC_NO_STDLIB
	MAT_LIBC_MALLOC,
#endif // RMALLOC_NO_STDLIB
};
typedef enum rm_alloc_type rm_alloc_t;

// TODO: poison values in the block on each side to detect memory overruns?
// TODO: we *could* add a size_t holding the requested allocation size. could be useful for dynamic memory size checks
struct rm_block {
	struct rm_block *next;
	size_t size;
	uint8_t free;
};
typedef struct rm_block rm_block_t;

struct rm_context {
	// how was this memory region allocated?
	rm_alloc_t alloc_type;

	// first block in the memory region
	// note: this MUST be the LAST element of this struct
	//       because all following bytes are part of the block
	rm_block_t first_block;
};

// combine all possible blocks in the free block list
static void rm_combine_blocks(rm_ctx_t *ctx) {
	rm_block_t *iter = &ctx->first_block;
	while (iter && iter->next) {
		if (iter->free && iter->next->free) {
			iter->size += sizeof(rm_block_t) + iter->next->size;
			iter->next = iter->next->next;
		} else {
			iter = iter->next;
		}
	}
}

// split the given block if it is reasonable to,
// keeping at least 'size' bytes in the block
// returns whether the block was split
static int rm_try_split_block(rm_block_t *block, size_t size) {
	size = ALIGN(size);

	// check if there is enough space for a new block in the leftover space
	// we add the ALIGNMENT value so any new block will have a size of at least ALIGNMENT
	// TODO: there is a case where the block after 'block' is free in which case the condition could just be 'block->size >= size'
	if (block->size >= size + sizeof(rm_block_t) + ALIGNMENT) {
		// get location of the new block
		rm_block_t *new_block = (rm_block_t *) ((uint8_t *)block + sizeof(rm_block_t) + size);

		// its new size is the leftover space from the old block minus this block's size
		new_block->size = block->size - size - sizeof(rm_block_t);
		new_block->free = 1;
		new_block->next = block->next;

		// if the block following the new_block is free, combine the two
		if (new_block->next && new_block->next->free) {
			new_block->size += sizeof(rm_block_t) + new_block->next->size;
			new_block->next = new_block->next->next;
		}

		// update this block's info
		block->size = size;
		block->next = new_block;

		return 1;
	} else {
		return 0;
	}
}

// try to grow the given block to at least the given size
static int rm_try_grow_block(rm_block_t *block, size_t size) {
	size = ALIGN(size);
	
	// while there's a free block ahead of this one, combine them
	while (block->next && block->next->free && block->size < size) {
		block->size += sizeof(rm_block_t) + block->next->size;
		block->next = block->next->next;
	}

	return block->size >= size;
}

// finds the smallest block that is at least 'size' bytes
// returns NULL on failure
static rm_block_t *rm_find_best_block(rm_ctx_t *ctx, size_t size) {
	rm_block_t *best_block = NULL;

	for (rm_block_t *iter = &ctx->first_block; iter; iter = iter->next) {
		if (iter->free && iter->size >= size) {
			if (best_block == NULL || iter->size < best_block->size) {
				best_block = iter;
			}
		}
	}

	return best_block;
}

// returns whether the given ptr points to a valid memory allocation
static int rm_is_ptr_valid(rm_ctx_t *ctx, void *ptr) {
	// we can't just do GET_BLOCK(ptr) and go from there because
	// ptr might be NULL or random and we don't know what might be there
	// instead we have to loop through the whole list to try to find its block

	rm_block_t *ptr_block = GET_BLOCK(ptr);

	// for the condition we use iter && iter <= ptr_block
	// because we know that if iter is greater than ptr_block we've gone
	// past where the pointer would be and didn't find it
	for (rm_block_t *iter = &ctx->first_block; iter && iter <= ptr_block; iter = iter->next) {
		if (iter == ptr_block)
			return 1;
	}

	return 0;
}

rm_ctx_t *rm_init_block(void *ptr, size_t size) {
	// note: explicit cast is needed for C++
	rm_ctx_t *ctx = (rm_ctx_t *) ptr;

	if (ctx) {
		ctx->alloc_type = MAT_USER_PROVIDED;
		ctx->first_block.size = size - sizeof(rm_ctx_t);
		ctx->first_block.free = 1;
		ctx->first_block.next = NULL;
	}

	return ctx;
}

#ifndef RMALLOC_NO_STDLIB
rm_ctx_t *rm_init_malloc(size_t size) {
	rm_ctx_t *ctx = rm_init_block(malloc(size), size);
	
	if (ctx) {
		ctx->alloc_type = MAT_LIBC_MALLOC;
	}

	return ctx;
}
#endif // RMALLOC_NO_STDLIB

void *rm_deinit(rm_ctx_t *ctx, size_t *size) {
	assert(ctx != NULL);

	if (size) *size = 0;

	// clear the ctx value so trying to use it causes segfault
	if (ctx) switch (ctx->alloc_type) {
	case MAT_USER_PROVIDED: {
		// calculate allocation size
		if (size) {
			// include the size of the context but rmove the first block's size,
			// that will be accounted for in the loop
			*size = sizeof(rm_ctx_t) - sizeof(rm_block_t);

			for (rm_block_t *iter = &ctx->first_block; iter; iter = iter->next) {
				*size = *size + sizeof(rm_block_t) + iter->size;
			}
		}

		// clear ctx to guard against use-after free
		RMALLOC_MEMSET(ctx, 0, sizeof(*ctx));
	} break;

#ifndef RMALLOC_NO_STDLIB
	case MAT_LIBC_MALLOC: {
		// clear ctx to guard against use-after free
		RMALLOC_MEMSET(ctx, 0, sizeof(*ctx));

		// free associated memory and clear ctx so NULL is returned
		free(ctx);
		ctx = NULL;
	} break;
#endif // RMALLOC_NO_STDLIB
	}

	
	return ctx;
}

void *rm_alloc(rm_ctx_t *ctx, size_t size) {
	assert(ctx != NULL);

	size = ALIGN(size);
	rm_block_t *block = rm_find_best_block(ctx, size);

	if (block) {
		// try to avoid taking up too much space if possible
		rm_try_split_block(block, size);
		
		block->free = 0;
		return block + 1;
	} else {
		// out of memory
		return NULL;
	}

}

void rm_free(rm_ctx_t *ctx, void *ptr)  {
	assert(ctx != NULL);
	if (ptr) {
		assert(rm_is_ptr_valid(ctx, ptr));

		GET_BLOCK(ptr)->free = 1;
		// note: we can't just do the same thing we did in rm_try_split_block
		// where we combine with the next block. that won't work if there's
		// multiple blocks to recombine or if there's free blocks before the
		// freed region.
		rm_combine_blocks(ctx);
	}
}

void *rm_calloc(rm_ctx_t *ctx, size_t count, size_t size) {
	assert(ctx != NULL);

	void *ptr = rm_alloc(ctx, count * size);

	if (ptr) {
		RMALLOC_MEMSET(ptr, 0, count * size);
	}

	return ptr;
}

void *rm_realloc(rm_ctx_t *ctx, void *ptr, size_t new_size) {
	assert(ctx != NULL);

	// passing ptr = NULL to m_realloc is the same as m_alloc(new_size)
	if (ptr == NULL) return rm_alloc(ctx, new_size);

	rm_block_t *block = GET_BLOCK(ptr);

	// easy case, block stays the same size
	if (new_size == block->size) {
		return ptr;
	}
	// if we are shrinking the allocation, try to split the current block.
	// either way the allocation will be in the same place so we return same ptr
	else if (new_size < block->size) {
		rm_try_split_block(block, new_size);
		return ptr;
	}
	// if we are growing the allocation, try to grow the current block.
	// if we successfully grow the block, try cutting it down to prevent waste
	else if (new_size > block->size && rm_try_grow_block(block, new_size)) {
		rm_try_split_block(block, new_size);
		return ptr;
	}
	// if all else fails, allocate a new block and copy the data over
	else {
		void *new_ptr = rm_alloc(ctx, new_size);

		if (new_ptr) {
			size_t copy_size = block->size < new_size ? block->size : new_size;
			RMALLOC_MEMCPY(new_ptr, ptr, copy_size);
			rm_free(ctx, ptr);
		}

		return new_ptr;
	}
}

#ifdef RMALLOC_DEBUG
void rm_debug(rm_ctx_t *ctx) {
	if (ctx) {
		size_t ctx_overhead = sizeof(rm_ctx_t) - sizeof(rm_block_t);
		printf(
			"m_debug: 0x%p - 0x%p (ctx ) (%d bytes)\n",
			ctx,
			ctx + ctx_overhead,
			ctx_overhead
		);
		for (rm_block_t *iter = &ctx->first_block; iter; iter = iter->next) {
			printf(
				"m_debug: 0x%p - 0x%p (%s) (%d + %d bytes)\n",
				iter,
				iter + iter->size + sizeof(rm_block_t),
				iter->free ? "free" : "used",
				sizeof(rm_block_t),
				iter->size
			);
		}
	} else {
		printf("m_debug: NULL\n");
	}
}
#endif // RMALLOC_DEBUG

#endif // RMALLOC_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // RMALLOC_H
