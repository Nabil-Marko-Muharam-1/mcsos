#include <stdint.h>
#include <stddef.h>
#include <mcsos/mm/pmm.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>

#define PAGE_SIZE 4096u
#define ALIGN_UP(addr, align) (((addr) + (align) - 1) & ~((align) - 1))

/* Request Peta Memori dari Limine */
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

/* Request HHDM Offset dari Limine */
__attribute__((used, section(".requests")))
volatile struct {
    uint64_t id[4];
    uint64_t revision;
    uint64_t offset;
} limine_hhdm_request = {
    .id = {0xc7b1dd30df4c8b88, 0x0a82e883a194f07b, 0x48dcb1bb8f115609, 0xb907d1d33f5fbbc7},
    .revision = 0,
    .offset = 0
};

#define LIMINE_MEMMAP_USABLE 0

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

/* Variabel Internal PMM */
static uint64_t highest_physical_address = 0;
static uint64_t total_page_frames = 0;
static uint64_t bitmap_size_bytes = 0;
static uint8_t *bitmap = (uint8_t*)0;

/* --- Fungsi Pembantu Internal --- */

static void pmm_memset(void *ptr, uint8_t value, uint64_t num) {
    uint8_t *p = (uint8_t *)ptr;
    for (uint64_t i = 0; i < num; i++) {
        p[i] = value;
    }
}

/* Mengubah bit menjadi 1 (Menandai memori terpakai/dikunci) */
static void bitmap_set(uint64_t frame_index) {
    uint64_t idx = frame_index / 8;
    uint64_t bit = frame_index % 8;
    bitmap[idx] |= (1 << bit);
}

/* Mengubah bit menjadi 0 (Menandai memori bebas/siap pakai) */
static void bitmap_clear(uint64_t frame_index) {
    uint64_t idx = frame_index / 8;
    uint64_t bit = frame_index % 8;
    bitmap[idx] &= ~(1 << bit);
}

/* Memeriksa status bit (Mengembalikan nilai true jika bit bernilai 1) */
static int bitmap_test(uint64_t frame_index) {
    uint64_t idx = frame_index / 8;
    uint64_t bit = frame_index % 8;
    return (bitmap[idx] & (1 << bit)) != 0;
}

/* --- Implementasi Fungsi Utama PMM --- */

void pmm_init(void) {
    log_writeln("[M5] Memulai inisialisasi Physical Memory Manager...");

    struct limine_memmap_response *memmap = limine_memmap_request.response;
    if (memmap == (void*)0) KERNEL_PANIC("Limine tidak memberikan memory map!", 0);

    /* Pass 1: Identifikasi Batas Atas Memori Fisik */
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        uint64_t top_address = entry->base + entry->length;
        if (top_address > highest_physical_address) {
            highest_physical_address = top_address;
        }
    }

    total_page_frames = highest_physical_address / PAGE_SIZE;
    bitmap_size_bytes = ALIGN_UP(total_page_frames / 8, PAGE_SIZE);

    /* Pass 2: Alokasikan Ruang untuk Menyimpan Array Bitmap */
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= bitmap_size_bytes) {
            bitmap = (uint8_t *)(entry->base + limine_hhdm_request.offset);
            
            /* Sita memori ini agar tidak ikut dibebaskan pada Pass 3 */
            entry->base += bitmap_size_bytes;
            entry->length -= bitmap_size_bytes;
            break;
        }
    }

    if (bitmap == (uint8_t*)0) {
        KERNEL_PANIC("Gagal mengalokasikan ruang untuk PMM Bitmap!", 0);
    }

    /* Kunci seluruh frame terlebih dahulu (Set seluruh bit menjadi 1) */
    pmm_memset(bitmap, 0xFF, bitmap_size_bytes);

    /* Pass 3: Buka Gembok (Clear bit ke 0) Khusus untuk RAM Berstatus USABLE */
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            for (uint64_t j = 0; j < entry->length; j += PAGE_SIZE) {
                uint64_t frame_index = (entry->base + j) / PAGE_SIZE;
                bitmap_clear(frame_index);
            }
        }
    }

    log_writeln("[M5] Bitmap PMM berhasil dikonfigurasi dan siap digunakan.");
}

void* pmm_alloc_page(void) {
    /* Cari bit 0 pertama dari awal frame */
    for (uint64_t i = 0; i < total_page_frames; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i); /* Kunci frame karena sudah disewa */
            return (void*)(i * PAGE_SIZE); /* Kembalikan alamat fisik halaman */
        }
    }
    return (void*)0; /* Out of Memory */
}

void pmm_free_page(void* page) {
    uint64_t phys_addr = (uint64_t)page;
    uint64_t frame_index = phys_addr / PAGE_SIZE;
    bitmap_clear(frame_index); /* Buka kembali gembok frame */
}
