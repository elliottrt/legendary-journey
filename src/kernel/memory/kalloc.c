#include "kalloc.h"
#include "virtmem.h"
#include "graphics/printf.h"
#include "std.h"
#include "mmu.h"

struct run {
    struct run *next;
};

static struct run *freemem;
static uint32_t available;
static uint32_t total;

void *kalloc(void) {
    struct run *allocated = freemem;
    freemem = freemem->next;

    available--;

    return (void *) allocated;
}

void kfree(void *address) {

    if ((uint32_t) address % PGSIZE) {
        printf("error: unaligned address %p to kfree\n", address);
        while(1);
    }

    if (address < (void *) end) {
        printf("error: low invalid address %p to kfree\n", address);
        while(1);
    }

    if (V2P(address) >= PHYSTOP) {
        printf("error: high invalid address %p to kfree\n", address);
        while(1);
    }

    memset(address, 0x00, PGSIZE);
        
    struct run *deallocated = (struct run *) address;

    deallocated->next = freemem;
    freemem = deallocated;

    available++;
}

void kfreerange(void *start, void *end) {
    int8_t *pstart = (int8_t *) PGROUNDUP((uint32_t) start);
    for(; pstart + PGSIZE <= (int8_t *) end; pstart += PGSIZE, total++) 
        kfree(pstart);
}

void kallocinit(void) {
    kfreerange(end, P2V(4*1024*1024));
}

void kallocexpand(void) {
    kfreerange(P2V(4*1024*1024), P2V(PHYSTOP));
}

uint32_t kallocavailable(void) {
    return available;
}

uint32_t kalloctotal(void) {
    return total;
}
