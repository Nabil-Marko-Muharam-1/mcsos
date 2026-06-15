#include <stdint.h>
#include "mcsos/fs/ramfs.h"
#include "mcsos/fs/fd.h"

extern void pmm_init();
extern void vmm_init();
extern void heap_init();
extern void gdt_init();
extern void x86_64_idt_init();
extern void syscall_init();
extern void load_and_run_user_program(void);

void kmain() {
    pmm_init();
    vmm_init();
    heap_init();
    gdt_init();
    x86_64_idt_init();
    
    // Aktifkan mesin M8
    ramfs_init();
    fd_init();
    syscall_init();

    // Lepaskan kendali ke program Ring 3
    load_and_run_user_program();
    
    while(1) __asm__ volatile("cli; hlt");
}
