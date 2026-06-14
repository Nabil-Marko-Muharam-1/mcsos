#include <mcsos/mm/heap.h>
#include <mcsos/mm/vmm.h>
#include <mcsos/mm/pmm.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>

/* Kita siapkan lahan virtual khusus untuk Heap di alamat tinggi yang sangat aman */
#define HEAP_START_VADDR   0xFFFFD00000000000
#define HEAP_INITIAL_PAGES 16  /* Kita alokasikan 64 KB pertama untuk Heap */

struct heap_block {
    uint64_t size;
    uint8_t is_free;
    struct heap_block *next;
} __attribute__((packed));

static struct heap_block *heap_head = (void*)0;

void heap_init(void) {
    log_writeln("[M5] Memulai inisialisasi Kernel Heap...");
    
    uint64_t *pml4 = vmm_get_active_pml4();
    
    /* 1. Sewa 16 Halaman (64 KB) dari PMM dan VMM */
    for (uint64_t i = 0; i < HEAP_INITIAL_PAGES; i++) {
        void *phys = pmm_alloc_page();
        if (!phys) KERNEL_PANIC("PMM kehabisan memori untuk Heap!", 0);
        vmm_map_page(pml4, HEAP_START_VADDR + (i * 4096), (uint64_t)phys, PAGE_PRESENT | PAGE_WRITE);
    }

    /* 2. Format 64 KB tersebut menjadi satu blok kosong raksasa */
    heap_head = (struct heap_block *)HEAP_START_VADDR;
    heap_head->size = (HEAP_INITIAL_PAGES * 4096) - sizeof(struct heap_block);
    heap_head->is_free = 1;
    heap_head->next = (void*)0;
    
    log_writeln("[M5] Kernel Heap (64 KB) berhasil di-mount!");
}

void* kmalloc(uint64_t size) {
    if (size == 0) return (void*)0;

    struct heap_block *curr = heap_head;
    while (curr != (void*)0) {
        if (curr->is_free && curr->size >= size) {
            
            /* Jika sisa memori di blok ini masih luas, kita "belah" bloknya */
            if (curr->size > size + sizeof(struct heap_block) + 8) {
                struct heap_block *new_block = (struct heap_block *)((uint64_t)curr + sizeof(struct heap_block) + size);
                new_block->size = curr->size - size - sizeof(struct heap_block);
                new_block->is_free = 1;
                new_block->next = curr->next;
                
                curr->next = new_block;
                curr->size = size;
            }
            
            curr->is_free = 0; /* Kunci memori */
            return (void*)((uint64_t)curr + sizeof(struct heap_block));
        }
        curr = curr->next;
    }
    
    KERNEL_PANIC("Kernel Heap Out of Memory!", 0);
    return (void*)0;
}

void kfree(void *ptr) {
    if (!ptr) return;
    
    /* Buka gembok blok memori */
    struct heap_block *block = (struct heap_block *)((uint64_t)ptr - sizeof(struct heap_block));
    block->is_free = 1;
    
    /* Garbage Collection Sederhana: Gabungkan blok-blok kosong yang berdekatan */
    struct heap_block *curr = heap_head;
    while (curr != (void*)0) {
        if (curr->is_free && curr->next != (void*)0 && curr->next->is_free) {
            curr->size += sizeof(struct heap_block) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}
