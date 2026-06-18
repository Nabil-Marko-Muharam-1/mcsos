#include "mcsos/security/sec.h"
#include "mcsos/fs/fd.h"

static uint32_t current_uid = 0; // Default: 0 = ROOT, 1 = GUEST

static void print_log(const char *msg) {
    int len = 0;
    while(msg[len]) len++;
    fd_write(1, (uint8_t *)msg, len);
}

void sec_set_current_uid(uint32_t uid) {
    current_uid = uid;
}

uint32_t sec_get_current_uid() {
    return current_uid;
}

// Mesin ACL (Access Control List)
int sec_check_file_access(uint32_t file_uid, uint32_t file_perms, uint32_t requested_access) {
    // Aturan 1: Root (UID 0) memiliki akses absolut ke segalanya
    if (current_uid == 0) return 1; 
    
    // Aturan 2: Pemilik file diizinkan
    if (current_uid == file_uid) return 1; 
    
    // Aturan 3: Periksa permission mask (Misal: hanya boleh Read)
    if ((file_perms & requested_access) == requested_access) return 1;
    
    print_log("[SEC] Akses DITOLAK! Pelanggaran Hak Akses (ACL).\n");
    return 0; // Blokir!
}

// Mesin Hardening & Validasi Pointer
int sec_validate_pointer(void *ptr, uint32_t size) {
    (void)size;
    if (ptr == 0) {
        print_log("[SEC] HARDENING: Upaya dereference Null Pointer diblokir!\n");
        return 0;
    }
    
    // Alamat kernel x86_64 biasanya dimulai dari 0xffffffff80000000
    // User Space tidak boleh mengirim pointer dari area memori ini ke Syscall!
    uint64_t addr = (uint64_t)ptr;
    if (addr >= 0xffffffff80000000) {
        print_log("[SEC] HARDENING: Upaya User membaca/menulis memori Kernel diblokir!\n");
        return 0;
    }
    
    return 1; // Pointer aman
}
