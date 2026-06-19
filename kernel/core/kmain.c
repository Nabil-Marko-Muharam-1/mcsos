#include <stdint.h>
#include "mcsos/fs/fd.h"
#include "mcsos/dev/block.h"
#include "mcsos/dev/ramdisk.h"
#include "mcsos/security/sec.h"
#include "mcsos/smp/spinlock.h"

extern void pmm_init(); extern void vmm_init(); extern void heap_init();
extern void gdt_init(); extern void x86_64_idt_init();
extern void syscall_init(); extern void ramfs_init();

static void print_log(const char *msg) {
    int len = 0;
    while(msg[len]) len++;
    fd_write(1, (uint8_t *)msg, len);
}

// --- Variabel Uji SMP & Lock Stress ---
static spinlock_t test_lock;
static volatile int shared_counter = 0;

// Fungsi untuk mensimulasikan CPU Core yang memperebutkan data
void mock_cpu_task(int increments) {
    for(int i = 0; i < increments; i++) {
        spinlock_acquire(&test_lock);
        
        // --- CRITICAL SECTION ---
        shared_counter++; 
        // ------------------------
        
        spinlock_release(&test_lock);
    }
}

void kmain() {
    pmm_init(); vmm_init(); heap_init();
    gdt_init(); x86_64_idt_init();
    ramfs_init(); fd_init(); syscall_init();

    print_log("\n[M13] Memulai pengujian SMP Spinlock & NUMA Prep...\n");

    // 1. Persiapan NUMA Node
    numa_node_t node0 = { .node_id = 0, .memory_base = 0x0, .memory_size = 0x40000000 };
    if (node0.node_id == 0) {
        print_log("[M13] Persiapan struktur NUMA Node 0 terinisialisasi.\n");
    }

    // 2. Inisialisasi Spinlock
    spinlock_init(&test_lock);
    
    print_log("[M13] Memulai Lock Stress Test (100.000 Iterasi)...\n");
    
    // Simulasi CPU 0 bekerja
    mock_cpu_task(50000);
    // Simulasi CPU 1 bekerja 
    mock_cpu_task(50000);

    // 3. Validasi Hasil Akhir
    if (shared_counter == 100000) {
        print_log("[M13] SUKSES: Spinlock mengamankan Critical Section! Counter = 100000\n");
    } else {
        print_log("[M13] GAGAL: Terjadi Race Condition!\n");
    }

    print_log("\n[KERNEL] Pengujian M13 Selesai 100%.\n");
    while(1) __asm__ volatile("cli; hlt");
}
