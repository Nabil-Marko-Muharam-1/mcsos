#ifndef MCSOS_MM_VMM_H
#define MCSOS_MM_VMM_H
#include <stdint.h>
#include <stddef.h>

/* Flag Standar untuk Page Table x86_64 */
#define PAGE_PRESENT (1 << 0)
#define PAGE_WRITE   (1 << 1)
#define PAGE_USER    (1 << 2)

void vmm_init(void);
void vmm_map_page(uint64_t *pml4, uint64_t vaddr, uint64_t paddr, uint64_t flags);
uint64_t* vmm_get_active_pml4(void);

#endif
