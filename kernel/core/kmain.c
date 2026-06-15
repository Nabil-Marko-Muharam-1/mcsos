#include <stdint.h>
#include "mcsos/fs/fd.h"
#include "mcsos/dev/block.h"
#include "mcsos/dev/ramdisk.h"

extern void pmm_init();
extern void vmm_init();
extern void heap_init();
extern void gdt_init();
extern void x86_64_idt_init();
extern void syscall_init();
extern void ramfs_init();

// Fungsi bantuan sederhana untuk mencetak teks ke layar via STDOUT (FD 1)
static void print_log(const char *msg) {
    int len = 0;
    while(msg[len]) len++;
    fd_write(1, (uint8_t *)msg, len);
}

void kmain() {
    pmm_init();
    vmm_init();
    heap_init();
    gdt_init();
    x86_64_idt_init();
    
    // Inisialisasi Subsistem M8
    ramfs_init();
    fd_init();
    syscall_init();

    // Inisialisasi Subsistem M9
    block_dev_init();
    ramdisk_init();

    print_log("\n[M9] Manajer Perangkat Blok dan Ramdisk diinisialisasi.\n");

    // Minta OS mencari disk di "port" pertama (Index 0)
    block_device_t *disk = block_dev_get(0);
    
    if (disk) {
        print_log("[M9] Berhasil mendeteksi disk: ");
        print_log(disk->name);
        print_log("\n");

        // 1. Siapkan memori kosong sebesar 1 BLOK (512 Byte)
        uint8_t write_buffer[512];
        for (int i = 0; i < 512; i++) write_buffer[i] = 0;
        
        // 2. Isi blok tersebut dengan pesan rahasia
        char *secret = "Halo TAPQI! Ini adalah data mentah dari Sektor 0 (LBA 0) Hardisk Virtual M9!\n";
        int i = 0;
        while(secret[i]) {
            write_buffer[i] = secret[i];
            i++;
        }

        // 3. Tulis 1 blok data ke LBA 0 (Sektor pertama disk)
        disk->write_blocks(disk, 0, 1, write_buffer);
        print_log("[M9] Menulis 1 Blok (512 Byte) ke LBA 0 sukses.\n");

        // 4. Siapkan wadah kosong baru untuk membuktikan kita benar-benar membacanya dari disk
        uint8_t read_buffer[512];
        for (int j = 0; j < 512; j++) read_buffer[j] = 0;

        // 5. Baca 1 blok dari LBA 0 ke dalam read_buffer
        disk->read_blocks(disk, 0, 1, read_buffer);
        
        print_log("[M9] Data yang dibaca dari disk LBA 0:\n>> ");
        print_log((char *)read_buffer);
    } else {
        print_log("[M9] ERROR: Tidak ada disk yang terdeteksi!\n");
    }

    print_log("[KERNEL] Pengujian M9 Selesai 100%.\n");
    
    while(1) __asm__ volatile("cli; hlt");
}
