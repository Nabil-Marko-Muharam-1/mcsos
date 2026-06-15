#include <stdint.h>
#include "mcsos/arch/x86_64/msr.h"
extern void syscall_entry(void);

static void sys_print(const char *str) {
    while (*str) {
        __asm__ volatile ("outb %0, %1" : : "a"(*str), "Nd"((uint16_t)0x3F8));
        str++;
    }
}

void syscall_handler(uint64_t syscall_nr, uint64_t arg1, uint64_t arg2) {
    (void)arg2;
    if (syscall_nr == 1) {
        sys_print((const char *)arg1);
    } else if (syscall_nr == 2) {
        sys_print("[KERNEL] Program User memanggil Exit. M7 Sukses 100%!\n");
        while(1) { __asm__ volatile("cli; hlt"); }
    }
}

void syscall_init(void) {
    uint64_t efer = rdmsr(IA32_EFER);
    wrmsr(IA32_EFER, efer | 1);
    wrmsr(IA32_LSTAR, (uint64_t)&syscall_entry);
    wrmsr(IA32_FMASK, 0x200);
}
