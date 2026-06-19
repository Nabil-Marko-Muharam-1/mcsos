#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>

// Struktur Gembok Keras (Spinlock)
typedef struct {
    volatile uint32_t locked; // 0 = Bebas, 1 = Dikunci
    uint32_t cpu_id;          // Mengingat CPU mana yang memegang kunci ini
} spinlock_t;

void spinlock_init(spinlock_t *lock);
void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);

// Struktur Persiapan NUMA (Non-Uniform Memory Access)
typedef struct {
    uint32_t node_id;
    uint64_t memory_base;
    uint64_t memory_size;
} numa_node_t;

#endif
