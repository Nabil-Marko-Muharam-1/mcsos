#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include <stddef.h>

// Representasi dari satu perangkat penyimpanan fisik/virtual (Disk)
typedef struct block_device {
    char name[32];             // Nama disk (misal: "ramdisk0", "sda")
    uint64_t block_size;       // Ukuran satu blok (standar: 512 byte)
    uint64_t total_blocks;     // Total kapasitas dalam jumlah blok
    
    // Function pointers untuk operasi disk mentah
    uint64_t (*read_blocks)(struct block_device *dev, uint64_t lba, uint64_t count, void *buffer);
    uint64_t (*write_blocks)(struct block_device *dev, uint64_t lba, uint64_t count, void *buffer);
    
    void *internal_data;       // Pointer untuk data spesifik driver (jika ada)
} block_device_t;

// API Subsistem Block Layer
void block_dev_init(void);
int block_dev_register(block_device_t *dev);
block_device_t *block_dev_get(int index);

#endif
