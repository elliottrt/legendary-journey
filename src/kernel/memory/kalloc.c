#include "kalloc.h"
#include "virtmem.h"
#include "graphics/printf.h"
#include "std.h"
#include "mmu.h"

struct run {
    struct run *next;
};

static struct run *freemem;
static uint available;
static uint total;

char *kalloc(void) {
    struct run *allocated = freemem;
    freemem = freemem->next;

    available--;

    return (char *) allocated;
}

void kfree(char *address) {

    if ((uint) address % PGSIZE) {
        printferr();
        printf("error: unaligned address %p to kfree\n", address);
        printfstd();
        while(1);
    }

    if (address < end) {
        printferr();
        printf("error: low invalid address %p to kfree\n", address);
        printfstd();
        while(1);
    }

    if (V2P(address) >= PHYSTOP) {
        printferr();
        printf("error: high invalid address %p to kfree\n", address);
        printfstd();
        while(1);
    }

    memset(address, 0x00, PGSIZE);
        
    struct run *deallocated = (struct run *) address;

    deallocated->next = freemem;
    freemem = deallocated;

    available++;
}

void kfreerange(void *start, void *end) {
    char *pstart = (char *) PGROUNDUP((uint) start);
    for(; pstart + PGSIZE <= (char *) end; pstart += PGSIZE, total++) 
        kfree(pstart);
}

void kallocinit(void) {
    kfreerange(end, P2V(4*1024*1024));
}

void kallocexpand(void) {
    kfreerange(P2V(4*1024*1024), P2V(PHYSTOP));
}

uint kallocavailable(void) {
    return available;
}

uint kalloctotal(void) {
    return total;
}
