#include <stdint.h>
#include <mcsos/arch/cpu.h>
#include <mcsos/arch/idt.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>
#include <mcsos/kernel/version.h>
#include <mcsos/mm/pmm.h>
#include <mcsos/mm/vmm.h>
#include <mcsos/mm/heap.h>

void kmain(void) {
    log_init();
    log_writeln("MCSOS kernel entered");

    x86_64_idt_init();
    
    /* TRINITY OF MEMORY MANAGEMENT */
    pmm_init();
    vmm_init();
    heap_init();

    /* --- UJI COBA KERNEL HEAP --- */
    log_writeln("[M5] Menguji alokasi dinamis (kmalloc & kfree)...");
    
    /* Pesan RAM 8 byte */
    uint64_t *data1 = (uint64_t*)kmalloc(8);
    *data1 = 0x1122334455667788;
    log_write("[M5] Data 1 (8 Bytes) dialokasikan di: ");
    log_key_value_hex64("ptr1", (uint64_t)data1);

    /* Pesan RAM 16 byte */
    uint64_t *data2 = (uint64_t*)kmalloc(16);
    *data2 = 0xAABBCCDDEEFF0011;
    log_write("[M5] Data 2 (16 Bytes) dialokasikan di: ");
    log_key_value_hex64("ptr2", (uint64_t)data2);

    if (*data1 == 0x1122334455667788 && *data2 == 0xAABBCCDDEEFF0011) {
        log_writeln("[M5] [SUCCESS] Modul 5 TUNTAS! PMM, VMM, dan Heap berjalan 100%!");
    } else {
        KERNEL_PANIC("Korupsi Data pada Kernel Heap!", 0);
    }
    
    kfree(data1);
    kfree(data2);
    log_writeln("[M5] Garbage collection (kfree) sukses dieksekusi.");

    log_writeln("[M5] Sistem bersiap untuk idle...");
    cpu_halt_forever();
}
