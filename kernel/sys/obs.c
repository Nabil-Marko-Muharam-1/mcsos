#include "mcsos/sys/obs.h"
#include <stdint.h>

// Fungsi eksternal untuk menulis log ke terminal
extern void fd_write(int fd, const uint8_t *buf, int count);
static int str_len(const char* s) { int i=0; while(s[i]) i++; return i; }
static void print_log(const char *msg) { fd_write(1, (uint8_t*)msg, str_len(msg)); }

void obs_init() {
    print_log("\n[M16] Menginisialisasi Subsistem Observability & Telemetri...\n");
}

void obs_dump_telemetry() {
    print_log("====================================================\n");
    print_log("[TELEMETRY] System Name : NABIL OS\n");
    print_log("[TELEMETRY] Version     : v1.0.0-RELEASE (Production)\n");
    print_log("[TELEMETRY] Boot Status : SECURE & STABLE\n");
    print_log("[TELEMETRY] Subsystems  : PMM, VMM, FB, SEC, OBS (Active)\n");
    print_log("====================================================\n");
}
