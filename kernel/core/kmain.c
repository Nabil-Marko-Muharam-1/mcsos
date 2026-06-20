#include <stdint.h>
#include "mcsos/fs/fd.h"
#include "mcsos/dev/block.h"
#include "mcsos/dev/ramdisk.h"
#include "mcsos/security/sec.h"
#include "mcsos/smp/spinlock.h"
#include "mcsos/dev/fb.h"
#include "mcsos/sec/container.h"

extern void pmm_init(); extern void vmm_init(); extern void heap_init();
extern void gdt_init(); extern void x86_64_idt_init();
extern void syscall_init(); extern void ramfs_init();

static void print_log(const char *msg) {
    int len = 0;
    while(msg[len]) len++;
    fd_write(1, (uint8_t *)msg, len);
}

void kmain() {
    pmm_init(); vmm_init(); heap_init();
    gdt_init(); x86_64_idt_init();
    ramfs_init(); fd_init(); syscall_init();

    // --- [M15] UJI COBA CONTAINER NAMESPACE ---
    container_init();
    container_test_simulation();
    // ------------------------------------------

    print_log("\n[M14] Menginisialisasi Framebuffer & Graphics Console...\n");

    int status = fb_init();
    
    if (status == 1) {
        print_log("[M14] SUKSES: Limine memberikan Framebuffer! Menggambar...\n");
        fb_draw_rect(50, 50, 100, 100, 0x00FF0000);
        fb_draw_rect(170, 50, 100, 100, 0x0000FF00);
        fb_draw_rect(290, 50, 100, 100, 0x000000FF);
        
        // Teks M14 Asli
        fb_draw_string("NABIL OS - M14 GRAPHICS CONSOLE OK!", 50, 180, 0x00FFFFFF, 0x00000000);
        
        // Teks M15 Baru (Warna Hijau Terang)
        fb_draw_string("[M15] OS-LEVEL CONTAINER ISOLATION: ACTIVE", 50, 200, 0x0000FF00, 0x00000000);
        
    } else if (status == -1) {
        print_log("[M14] GAGAL: Limine mengabaikan request Framebuffer (Response = NULL).\n");
    } else {
        print_log("[M14] GAGAL: Resolusi Layar tidak didukung oleh emulator QEMU.\n");
    }

    print_log("\n[KERNEL] Pengujian Selesai 100%. Lihat Jendela QEMU & Log Terminal!\n");
    while(1) __asm__ volatile("cli; hlt");
}
