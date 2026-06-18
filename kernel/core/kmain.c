#include <stdint.h>
#include "mcsos/fs/fd.h"
#include "mcsos/dev/block.h"
#include "mcsos/dev/ramdisk.h"
#include "mcsos/fs/mcsfs.h"

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

    print_log("\n[M10] Memulai pengujian Integrasi VFS & MCSFS...\n");
    block_device_t *disk = block_dev_get(0);
    
    if (disk) {
        // 1. Format & Mount
        mcsfs_format(disk);
        mcsfs_mount(disk);

        // 2. Buat file baru di disk
        vfs_node_t *file = mcsfs_create_file(disk, "rahasia.txt");
        if (file) {
            // 3. Tulis teks ke file tersebut via abstraksi VFS
            char *text = "Halo TAPQI! Ini adalah pesan abadi dari Sistem Fail MCSFS!\n";
            int text_len = 0;
            while(text[text_len]) text_len++;
            
            // PERBAIKAN: Menambahkan (uint8_t *) untuk casting tipe data
            file->ops->write(file, 0, text_len, (uint8_t *)text);
            print_log("[M10] Sukses menulis data ke 'rahasia.txt' via VFS.\n");

            // 4. Mari kita buktikan bahwa data benar-benar mendarat di disk (LBA 2)
            // LBA 0 = Superblock, LBA 1 = Inode, LBA 2 = Data dari file pertama
            uint8_t read_buffer[512];
            for (int j=0; j<512; j++) read_buffer[j] = 0;
            
            disk->read_blocks(disk, 2, 1, read_buffer);
            print_log("[M10] Bukti pembacaan fisik disk dari LBA 2:\n>> ");
            print_log((char *)read_buffer);
        }
    }

    print_log("\n[KERNEL] Pengujian M10 Selesai 100%.\n");
    while(1) __asm__ volatile("cli; hlt");
}
