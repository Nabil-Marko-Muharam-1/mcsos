#include "mcsos/dev/block.h"

#define MAX_BLOCK_DEVICES 8 // OS kita sementara mendukung maksimal 8 disk terpasang

// Tabel daftar disk yang dikenali oleh OS
static block_device_t *block_devices[MAX_BLOCK_DEVICES];

void block_dev_init(void) {
    for (int i = 0; i < MAX_BLOCK_DEVICES; i++) {
        block_devices[i] = NULL;
    }
}

int block_dev_register(block_device_t *dev) {
    if (!dev) return -1;
    
    // Cari slot kosong (port SATA/NVMe virtual)
    for (int i = 0; i < MAX_BLOCK_DEVICES; i++) {
        if (block_devices[i] == NULL) {
            block_devices[i] = dev;
            return i; // Kembalikan ID disk
        }
    }
    return -1; // Gagal: tidak ada port kosong
}

block_device_t *block_dev_get(int index) {
    if (index >= 0 && index < MAX_BLOCK_DEVICES) {
        return block_devices[index];
    }
    return NULL;
}
