#include "mcsos/dev/ramdisk.h"
#include "mcsos/mm/heap.h"

#define RAMDISK_BLOCK_SIZE 512
#define RAMDISK_TOTAL_BLOCKS 64 // 64 x 512 byte = 32 KB

static uint8_t *ramdisk_memory;
static block_device_t ramdisk_dev;

// Fungsi helper internal untuk menyalin memori
static void *rd_memcpy(void *dest, const void *src, uint64_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;
    for (uint64_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

// Membaca 'count' blok dari LBA (Logical Block Address) ke buffer
static uint64_t ramdisk_read(block_device_t *dev, uint64_t lba, uint64_t count, void *buffer) {
    if (lba >= dev->total_blocks) return 0;
    
    uint64_t blocks_to_read = count;
    if (lba + count > dev->total_blocks) {
        blocks_to_read = dev->total_blocks - lba;
    }

    uint64_t offset = lba * dev->block_size;
    uint64_t bytes = blocks_to_read * dev->block_size;
    
    rd_memcpy(buffer, ramdisk_memory + offset, bytes);
    return blocks_to_read;
}

// Menulis 'count' blok dari buffer ke LBA (Logical Block Address)
static uint64_t ramdisk_write(block_device_t *dev, uint64_t lba, uint64_t count, void *buffer) {
    if (lba >= dev->total_blocks) return 0;
    
    uint64_t blocks_to_write = count;
    if (lba + count > dev->total_blocks) {
        blocks_to_write = dev->total_blocks - lba;
    }

    uint64_t offset = lba * dev->block_size;
    uint64_t bytes = blocks_to_write * dev->block_size;
    
    rd_memcpy(ramdisk_memory + offset, buffer, bytes);
    return blocks_to_write;
}

void ramdisk_init(void) {
    // 1. Sewa 32 KB dari Heap untuk "piringan" fisik disk kita
    ramdisk_memory = (uint8_t *)kmalloc(RAMDISK_BLOCK_SIZE * RAMDISK_TOTAL_BLOCKS);
    if (!ramdisk_memory) return; 

    // 2. Beri nama disk ini "ramdisk0"
    char *name = "ramdisk0";
    int i = 0;
    while(name[i] && i < 31) { ramdisk_dev.name[i] = name[i]; i++; }
    ramdisk_dev.name[i] = '\0';

    // 3. Pasang spesifikasi dan function pointers (Polimorfisme)
    ramdisk_dev.block_size = RAMDISK_BLOCK_SIZE;
    ramdisk_dev.total_blocks = RAMDISK_TOTAL_BLOCKS;
    ramdisk_dev.read_blocks = ramdisk_read;
    ramdisk_dev.write_blocks = ramdisk_write;
    ramdisk_dev.internal_data = ramdisk_memory;

    // 4. "Tancapkan" disk ini ke Manajer Block Layer OS
    block_dev_register(&ramdisk_dev);
}
