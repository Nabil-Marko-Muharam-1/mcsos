#include <stdint.h>
#include "mcsos/arch/x86_64/msr.h"
#include "mcsos/fs/fd.h"
#include "mcsos/fs/ramfs.h"

extern void syscall_entry(void);

// Handler sekarang mengembalikan uint64_t agar %rax di User Space mendapat balikan
uint64_t syscall_handler(uint64_t sys_nr, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    if (sys_nr == 0) {        // SYS_READ
        return fd_read((int)arg1, (uint8_t *)arg2, arg3);
    } else if (sys_nr == 1) { // SYS_WRITE
        return fd_write((int)arg1, (uint8_t *)arg2, arg3);
    } else if (sys_nr == 2) { // SYS_OPEN
        vfs_node_t *node = ramfs_create_file((const char *)arg1);
        if (!node) return -1;
        return fd_open(node); // Mengembalikan tiket FD ke aplikasi!
    } else if (sys_nr == 60) { // SYS_EXIT
        fd_write(1, (uint8_t *)"[KERNEL] Aplikasi User selesai dengan aman. M8 Sukses!\n", 55);
        while(1) { __asm__ volatile("cli; hlt"); }
    }
    return 0;
}

void syscall_init(void) {
    uint64_t efer = rdmsr(IA32_EFER);
    wrmsr(IA32_EFER, efer | 1);
    wrmsr(IA32_LSTAR, (uint64_t)&syscall_entry);
    wrmsr(IA32_FMASK, 0x200);
}
