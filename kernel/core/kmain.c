#include <stdint.h>
#include <mcsos/arch/cpu.h>
#include <mcsos/arch/idt.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>
#include <mcsos/kernel/version.h>
#include <mcsos/mm/pmm.h>

void kmain(void) {
    log_init();
    log_write(MCSOS_NAME);
    log_write(" ");
    log_write(MCSOS_VERSION);
    log_writeln(" kernel entered");

    x86_64_idt_init();
    pmm_init();

    /* --- PROSES SELF-TEST ALOKASI MEMORI M5 --- */
    log_writeln("[M5] Memulai Uji Alokasi Halaman Fisik (PMM Self-Test)...");

    // 1. Alokasikan halaman pertama
    void *page1 = pmm_alloc_page();
    log_write("[M5] Halaman 1 dialokasikan pada alamat fisik: ");
    log_key_value_hex64("page1_addr", (uint64_t)page1);

    // 2. Alokasikan halaman kedua
    void *page2 = pmm_alloc_page();
    log_write("[M5] Halaman 2 dialokasikan pada alamat fisik: ");
    log_key_value_hex64("page2_addr", (uint64_t)page2);

    // 3. Bebaskan halaman pertama
    pmm_free_page(page1);
    log_writeln("[M5] Halaman 1 berhasil dibebaskan.");

    // 4. Alokasikan lagi! Halaman ketiga harus menempati bekas alamat halaman pertama
    void *page3 = pmm_alloc_page();
    log_write("[M5] Halaman 3 dialokasikan (Harus sama dengan Halaman 1): ");
    log_key_value_hex64("page3_addr", (uint64_t)page3);

    if (page1 == page3 && page1 != page2) {
        log_writeln("[M5] [SUCCESS] PMM Alokator teruji stabil dan bekerja 100%!");
    } else {
        KERNEL_PANIC("PMM Self-Test Gagal! Logika bitwise korup.", 0);
    }

    log_writeln("[M5] Kernel masuk ke mode idle...");
    cpu_halt_forever();
}
