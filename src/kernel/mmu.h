#ifndef _KERNEL_MMU
#define _KERNEL_MMU

#define PDENTRIES 1024
#define PGENTRIES 1024
#define PGSIZE 4096

#define PGROUNDUP(a)  (((a)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#define V2P(a) (((uint) (a)) - KERNBASE)
#define P2V(a) ((void *)(((char *) (a)) + KERNBASE))
#define V2P_WO(x) ((x) - KERNBASE)
#define P2V_WO(x) ((x) + KERNBASE)

#define PTE_P           0x001   // Present
#define PTE_W           0x002   // Writeable
#define PTE_U           0x004   // User
#define PTE_PS          0x080   // Page Size

#define PTXSHIFT        12      // offset of PTX in a linear address
#define PDXSHIFT        22      // offset of PDX in a linear address

#define EXTMEM KERNEL_LOAD            // Start of extended memory

#define KERNLINK (KERNBASE+EXTMEM)  // Address where kernel is linked

#define NELEM(x) (sizeof(x)/sizeof((x)[0]))

#define PDX(va) (((uint)(va) >> PDXSHIFT) & 0x3FF)
#define PTX(va) (((uint)(va) >> PTXSHIFT) & 0x3FF)

#define PTE_ADDR(pte)   ((uint)(pte) & ~0xFFF)
#define PTE_FLAGS(pte)  ((uint)(pte) &  0xFFF)

#endif
