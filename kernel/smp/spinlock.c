#include "mcsos/smp/spinlock.h"

void spinlock_init(spinlock_t *lock) {
    lock->locked = 0;
    lock->cpu_id = (uint32_t)-1; // Belum ada yang pegang
}

// Fungsi untuk MEREBUT gembok
void spinlock_acquire(spinlock_t *lock) {
    // __sync_lock_test_and_set adalah instruksi atomik langsung ke CPU (Hardware level).
    // Jika lock masih 1 (dikunci orang lain), CPU akan berputar (spin) di sini.
    while (__sync_lock_test_and_set(&lock->locked, 1)) {
        // Instruksi 'pause' mencegah CPU kepanasan saat menunggu gembok terbuka (x86_64 specific)
        __asm__ volatile("pause");
    }
    // Simulasi: Tandai bahwa CPU 0 yang memegang gembok ini
    lock->cpu_id = 0; 
}

// Fungsi untuk MELEPAS gembok
void spinlock_release(spinlock_t *lock) {
    lock->cpu_id = (uint32_t)-1;
    // Lepaskan kunci secara atomik
    __sync_lock_release(&lock->locked);
}
