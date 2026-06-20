#include <stdint.h>
#include "mcsos/fs/fd.h"
#include "mcsos/dev/block.h"
#include "mcsos/dev/ramdisk.h"
#include "mcsos/security/sec.h"
#include "mcsos/smp/spinlock.h"
#include "mcsos/dev/fb.h"
#include "mcsos/sec/container.h"
#include "mcsos/sys/obs.h"

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

    // Uji M15 (Container)
    container_init();
    container_test_simulation();

    // --- [M16] OBSERVABILITY & READINESS REVIEW ---
    obs_init();
    obs_dump_telemetry();
    // ----------------------------------------------

    print_log("\n[M14] Menginisialisasi Framebuffer & Graphics Console...\n");
    int status = fb_init();
    
    if (status == 1) {
        print_log("[M14] SUKSES: Limine memberikan Framebuffer! Menggambar GUI...\n");
        fb_draw_rect(50, 50, 100, 100, 0x00FF0000);
        fb_draw_rect(170, 50, 100, 100, 0x0000FF00);
        fb_draw_rect(290, 50, 100, 100, 0x000000FF);
        
        fb_draw_string("NABIL OS - M14 GRAPHICS CONSOLE OK!", 50, 180, 0x00FFFFFF, 0x00000000);
        fb_draw_string("[M15] OS-LEVEL CONTAINER ISOLATION: ACTIVE", 50, 200, 0x0000FF00, 0x00000000);
        
        // Teks Final M16
        fb_draw_string("[M16] OBSERVABILITY & TELEMETRY : ONLINE", 50, 220, 0x00FFFF00, 0x00000000); // Warna Kuning/Emas
        fb_draw_string("=== NABIL OS v1.0 PRODUCTION READY ===", 50, 250, 0x00FF00FF, 0x00000000); // Warna Magenta
        
    } else {
        print_log("[KERNEL] GAGAL: Resolusi Layar tidak didukung.\n");
    }

    print_log("\n[NABIL OS v1.0] SYSTEM HALTED SECURELY. SIAP RILIS!\n");
    while(1) __asm__ volatile("cli; hlt");
}
