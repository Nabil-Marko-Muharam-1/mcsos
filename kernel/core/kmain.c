#include <stdint.h>
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
    
    /* Peta Memori dan Penahan Crash */
    gdt_init();
    x86_64_idt_init();
    
    /* Gerbang Syscall M7 */
    syscall_init();
    
    /* Eksekusi Program User */
    load_and_run_user_program();
    
    while(1) __asm__ volatile("cli; hlt");
}
