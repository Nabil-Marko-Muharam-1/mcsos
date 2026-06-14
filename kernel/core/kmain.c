#include <stdint.h>
#include <mcsos/arch/cpu.h>
#include <mcsos/arch/idt.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>
#include <mcsos/kernel/version.h>
#include <mcsos/mm/pmm.h>

extern char _kernel_start[];
extern char _kernel_end[];

void kmain(void) {
    log_init();
    log_write(MCSOS_NAME);
    log_write(" ");
    log_write(MCSOS_VERSION);
    log_writeln(" kernel entered");

    x86_64_idt_init();
    
    // Panggil inisialisasi modul memori M5
    pmm_init();

    log_writeln("[M5] Kernel masuk ke mode idle...");
    cpu_halt_forever();
}
