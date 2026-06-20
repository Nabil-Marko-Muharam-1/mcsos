#include "mcsos/sec/container.h"
#include "mcsos/fs/fd.h"

// Fungsi utilitas string sederhana
static int str_len(const char* s) { int i=0; while(s[i]) i++; return i; }
static void print_log(const char *msg) { fd_write(1, (uint8_t*)msg, str_len(msg)); }

struct container containers[MAX_CONTAINERS];

void container_init() {
    for(int i = 0; i < MAX_CONTAINERS; i++) {
        containers[i].active = 0;
    }
    print_log("[M15] Subsistem Container & Namespace berhasil diinisialisasi.\n");
}

int container_create(const char* root_path) {
    for(int i = 0; i < MAX_CONTAINERS; i++) {
        if(!containers[i].active) {
            containers[i].active = 1;
            containers[i].id = i + 1;
            
            // Salin path penjara (Jail Path)
            int j = 0;
            while(root_path[j] && j < 31) {
                containers[i].root_path[j] = root_path[j];
                j++;
            }
            containers[i].root_path[j] = '\0';
            return containers[i].id;
        }
    }
    return -1; // Penuh
}

// Simulasi perpindahan konteks Namespace
void container_test_simulation() {
    print_log("[M15] Memulai simulasi OS-Level Virtualization (Jail / Namespace)...\n");

    // Membuat 2 Container berbeda
    int c1 = container_create("/var/jail/web_server");
    int c2 = container_create("/var/jail/database");

    print_log("----------------------------------------------------\n");
    // Simulasi pandangan dari sisi OS Host
    print_log("[HOST OS]  Proses berjalan bebas di Root FS: /\n");

    // Simulasi pandangan dari sisi Container 1
    if(c1 != -1) {
        print_log("[CONT-1]   Proses terisolasi. Virtual Root FS: ");
        print_log(containers[c1-1].root_path);
        print_log("\n           PID Asli: 405 -> Virtual PID: 1 (Spoofed)\n");
    }

    // Simulasi pandangan dari sisi Container 2
    if(c2 != -1) {
        print_log("[CONT-2]   Proses terisolasi. Virtual Root FS: ");
        print_log(containers[c2-1].root_path);
        print_log("\n           PID Asli: 406 -> Virtual PID: 1 (Spoofed)\n");
    }
    print_log("----------------------------------------------------\n");
}
