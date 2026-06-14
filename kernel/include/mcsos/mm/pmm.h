#ifndef MCSOS_MM_PMM_H
#define MCSOS_MM_PMM_H
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096u

void pmm_init(void);
void* pmm_alloc_page(void);
void pmm_free_page(void* page);

#endif
