#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>

typedef volatile uint32_t spinlock_t;

static inline void spinlock_acquire(spinlock_t *lock) {
    // Berputar di sini sampai lock bernilai 0, lalu set menjadi 1 secara atomik
    while (__atomic_test_and_set(lock, __ATOMIC_ACQUIRE)) {
        __builtin_ia32_pause(); // Mencegah CPU overheat saat menunggu
    }
}

static inline void spinlock_release(spinlock_t *lock) {
    __atomic_clear(lock, __ATOMIC_RELEASE); // Kembalikan lock ke 0
}

#endif
