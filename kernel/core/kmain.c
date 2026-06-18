#include <stdint.h>
#include "mcsos/fs/fd.h"
#include "mcsos/dev/block.h"
#include "mcsos/dev/ramdisk.h"
#include "mcsos/fs/mcsfs.h"
#include "mcsos/net/net.h"

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

// --- Fungsi Aplikasi Tingkat Pengguna (User Space) ---
void aplikasi_chat_callback(uint8_t *data, uint32_t len) {
    (void)len; // Matikan warning unused parameter dari compiler
    print_log("\n==================================================\n");
    print_log(" [APLIKASI CHAT] Menerima sinyal dari Kernel!!!\n");
    print_log(" [APLIKASI CHAT] Isi Pesan: ");
    print_log((char *)data);
    print_log("\n==================================================\n");
}

void kmain() {
    pmm_init();
    vmm_init();
    heap_init();
    gdt_init();
    x86_64_idt_init();
    
    ramfs_init();
    fd_init();
    syscall_init();
    block_dev_init();
    ramdisk_init();

    print_log("\n[M11] Memulai pengujian Soket Jaringan...\n");

    // 1. Inisialisasi Jaringan
    net_init();

    // 2. Aplikasi mendaftarkan dirinya ke Port 8080
    int sock_id = udp_bind(8080, aplikasi_chat_callback);
    if (sock_id >= 0) {
        print_log("[M11] Aplikasi berhasil Listen di Port 8080 (Socket ID 0).\n");
    }

    // 3. Paket mentah masuk (Perhatikan baris ke-3, 0x1F90 adalah 8080 dalam Hex Big-Endian)
    uint8_t dummy_packet[] = {
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x08, 0x00,
        0x45, 0x00, 0x00, 0x2F, 0x00, 0x00, 0x00, 0x00, 0x40, 0x11, 0x00, 0x00, 0xC0, 0xA8, 0x01, 0x01, 0xC0, 0xA8, 0x01, 0x02,
        0x10, 0x92, 0x1F, 0x90, 0x00, 0x1B, 0x00, 0x00,
        'H', 'A', 'L', 'O', ' ', 'D', 'A', 'R', 'I', ' ', 
        'J', 'A', 'R', 'I', 'N', 'G', 'A', 'N', '\0'
    };

    print_log("[M11] Simulasi: NIC menangkap sinyal dan mengirimnya ke Kernel...\n");

    // 4. Kernel merutekan paket!
    net_parse_packet(dummy_packet, sizeof(dummy_packet));

    print_log("\n[KERNEL] Pengujian M11 Selesai 100%.\n");
    while(1) __asm__ volatile("cli; hlt");
}
