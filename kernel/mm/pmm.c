#include <stdint.h>
#include <mcsos/mm/pmm.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>

/* PERBAIKAN: Menambahkan Limine Common Magic yang benar! */
__attribute__((used, section(".requests")))
volatile struct {
    uint64_t id[4];
    uint64_t revision;
    void *response;
} limine_memmap_request = {
    .id = {0xc7b1dd30df4c8b88, 0x0a82e883a194f07b, 0x67cf3d9d378a806f, 0xe304acdfc50c3c62},
    .revision = 0,
    .response = (void*)0
};

#define LIMINE_MEMMAP_USABLE                 0
#define LIMINE_MEMMAP_RESERVED               1
#define LIMINE_MEMMAP_ACPI_RECLAIMABLE       2
#define LIMINE_MEMMAP_ACPI_NVS               3
#define LIMINE_MEMMAP_BAD_MEMORY             4
#define LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE 5
#define LIMINE_MEMMAP_KERNEL_AND_MODULES     6
#define LIMINE_MEMMAP_FRAMEBUFFER            7

struct limine_memmap_entry {
    uint64_t base;
    uint64_t length;
    uint64_t type;
};

struct limine_memmap_response {
    uint64_t revision;
    uint64_t entry_count;
    struct limine_memmap_entry **entries;
};

static uint64_t total_usable_memory = 0;

void pmm_init(void) {
    log_writeln("[M5] Memulai inisialisasi Physical Memory Manager...");

    struct limine_memmap_response *memmap = limine_memmap_request.response;
    if (memmap == (void*)0) {
        KERNEL_PANIC("Limine tidak memberikan memory map!", 0);
    }

    log_write("[M5] Ditemukan ");
    log_key_value_hex64("jumlah_entry", memmap->entry_count);

    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            total_usable_memory += entry->length;
        }
    }

    log_write("[M5] Total Usable RAM (Bytes): ");
    log_key_value_hex64("usable_ram_hex", total_usable_memory);
    log_writeln("[M5] PMM mapping dasar selesai.");
}

void* pmm_alloc_page(void) {
    return (void*)0; 
}

void pmm_free_page(void* page) {
    (void)page;
}
