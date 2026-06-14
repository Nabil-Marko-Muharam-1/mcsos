#ifndef MCSOS_MM_HEAP_H
#define MCSOS_MM_HEAP_H
#include <stdint.h>
#include <stddef.h>

void heap_init(void);
void* kmalloc(uint64_t size);
void kfree(void *ptr);

#endif
