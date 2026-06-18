#include <stdint.h>
#include "mcsos/fs/fd.h"
#include "mcsos/dev/block.h"
#include "mcsos/dev/ramdisk.h"
#include "mcsos/security/sec.h"

extern void pmm_init();
extern void vmm_init();
extern void heap_init();
extern void gdt_init();
extern void x86_64_idt_init();
extern void syscall_init();
extern void ramfs_init();

static void print_log(const char *msg) {
    int len = 0;
    while(msg[len]) len++;
    fd_write(1, (uint8_t *)msg, len);
}

// Simulasi Syscall Write yang sudah di-Harden
int sys_write_mock(int fd, void *buffer, uint32_t size) {
    (void)fd;
    // BOUNDARY SECURITY CHECK
    if (!sec_validate_pointer(buffer, size)) return -1;
    
    print_log("[SYSCALL] Syscall dieksekusi dengan aman.\n");
    return 0; 
}

void kmain() {
    pmm_init(); vmm_init(); heap_init();
    gdt_init(); x86_64_idt_init();
    ramfs_init(); fd_init(); syscall_init();

    print_log("\n[M12] Memulai pengujian Security Model, ACL, dan Hardening...\n");

    // --- TEST 1: ACL / CAPABILITY ---
    print_log("\n--- TEST 1: Access Control List (ACL) ---\n");
    uint32_t secret_file_uid = 0; // File ini milik Root
    uint32_t secret_file_perms = SEC_ACCESS_READ; // Hanya bisa dibaca
    
    sec_set_current_uid(1); // Downgrade hak akses menjadi GUEST (UID 1)
    print_log("[M12] Status: User saat ini = Guest (UID 1).\n");
    print_log("[M12] Guest mencoba melakukan tindakan WRITE ke file Root...\n");
    
    if (sec_check_file_access(secret_file_uid, secret_file_perms, SEC_ACCESS_WRITE)) {
         print_log("[M12] ERROR FATAL: Guest berhasil meretas file!\n");
    } else {
         print_log("[M12] SUKSES: Kernel OS berhasil memblokir aksi ilegal Guest.\n");
    }

    sec_set_current_uid(0); // Upgrade hak akses menjadi ROOT (UID 0)
    print_log("[M12] Status: User saat ini = Root (UID 0).\n");
    if (sec_check_file_access(secret_file_uid, secret_file_perms, SEC_ACCESS_WRITE)) {
         print_log("[M12] SUKSES: Root diizinkan menulis ke filenya sendiri.\n");
    }

    // --- TEST 2: SYSCALL FUZZING & HARDENING ---
    print_log("\n--- TEST 2: Syscall Fuzzing & Memory Hardening ---\n");
    
    print_log("[M12] Fuzzer mencoba menginjeksi Null Pointer ke Syscall...\n");
    sys_write_mock(1, (void *)0, 100);

    print_log("[M12] Fuzzer mencoba menginjeksi Pointer Kernel ke Syscall...\n");
    sys_write_mock(1, (void *)0xffffffff80006000, 100);

    print_log("\n[KERNEL] Pengujian M12 Selesai 100%.\n");
    while(1) __asm__ volatile("cli; hlt");
}
