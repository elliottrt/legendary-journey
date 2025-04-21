#include "kernel/memory/general_alloc.h"

#include "common/std.h"

#define GET_BLOCK(P) (((block_t *) (P)) - 1)

#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

typedef struct block {
	size_t size;
	struct block *next;
	bool free;
} block_t;

static block_t *free_list = NULL;

void gen_alloc_init(void *ptr, size_t size) {

	free_list = ptr;

	free_list->size = size - sizeof(block_t);
	free_list->free = true;
	free_list->next = NULL;

}

void *gen_malloc(size_t size) {

	size = ALIGN(size);
	block_t *iter = free_list;
	block_t *best_block = NULL;

	// find the smallest block that is big enough

	while (iter) {

		if (iter->free && iter->size >= size) {

			if (best_block == NULL || iter->size < best_block->size) {

				best_block = iter;

			}

		}

		iter = iter->next;

	}

	// if we found a block

	if (best_block) {

		// split block if needed

		if (best_block->size >= size + sizeof(block_t) + ALIGNMENT) {

			block_t *new_block = (block_t *) ((uint8_t *)best_block + sizeof(block_t) + size);
			new_block->size = best_block->size - size - sizeof(block_t);
			new_block->free = true;
			new_block->next = best_block->next;

			best_block->size = size;
			best_block->next = new_block;
		}
		
		best_block->free = false;
		return best_block + 1;

	}

	// out of memory

	return NULL;

}

void gen_free(void *ptr)  {

	if (ptr) {

		block_t *block = GET_BLOCK(ptr);
		block->free = 1;

		// combine adjacent blocks

		block_t *iter = free_list;

		while (iter && iter->next) {

			if (iter->free && iter->next->free) {

				iter->size += iter->next->size + sizeof(block_t);
				iter->next = iter->next->next;

			} else {

				iter = iter->next;

			}

		}

	}

}

void *gen_calloc(size_t count, size_t size) {
	void *ptr = gen_malloc(count * size);

	if (ptr) {
		memset(ptr, 0, count * size);
	}

	return ptr;
}

void *gen_realloc(void *ptr, size_t new_size) {

	block_t *block = GET_BLOCK(ptr);

	if (new_size == block->size) {
		return ptr;
	}

	void *new_ptr = gen_malloc(new_size);

	if (ptr && new_ptr) {

		memcpy(new_ptr, ptr, new_size);

		gen_free(ptr);

	}

	return new_ptr;

}

#ifdef GEN_ALLOC_DEBUG
#include "kernel/graphics/printf.h"

void gen_debug(void) {
	if (free_list) {
		block_t *iter = free_list;
		while (iter) {
			printf(
				"gen_debug: 0x%p - 0x%p (%s) (%u + %u bytes)\n",
				(uintptr_t) iter,
				(uintptr_t) iter + iter->size + sizeof(block_t),
				iter->free ? "free" : "used",
				sizeof(block_t),
				iter->size
			);
			iter = iter->next;
		}
	} else {
		printf("gen_debug: NULL\n");
	}
}
#endif
