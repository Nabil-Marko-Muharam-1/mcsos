#include <mcsos/mm/vmm.h>
#include <mcsos/mm/pmm.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>

/* PERBAIKAN: Jangan buat request baru. Pinjam request HHDM milik pmm.c menggunakan 'extern' */
extern volatile struct {
    uint64_t id[4];
    uint64_t revision;
    uint64_t offset;
} limine_hhdm_request;

static uint64_t hhdm_offset = 0;
static uint64_t *current_pml4 = (uint64_t*)0;

/* Membaca register CR3 untuk mendapatkan tabel utama (PML4) */
static inline uint64_t read_cr3(void) {
    uint64_t val;
    __asm__ volatile("mov %%cr3, %0" : "=r"(val) : : "memory");
    return val;
}

static void vmm_memset(void *ptr, uint8_t value, uint64_t num) {
    uint8_t *p = (uint8_t *)ptr;
    for (uint64_t i = 0; i < num; i++) p[i] = value;
}

void vmm_init(void) {
    log_writeln("[M5] Memulai inisialisasi Virtual Memory Manager (VMM)...");
    
    /* Menggunakan offset dari pmm.c */
    hhdm_offset = limine_hhdm_request.offset;
    
    /* Ambil tabel halaman yang sedang dipakai CPU */
    uint64_t cr3_phys = read_cr3() & ~0xFFF;
    current_pml4 = (uint64_t*)(cr3_phys + hhdm_offset);
    
    log_write("[M5] Active CR3 (PML4) physical: ");
    log_key_value_hex64("cr3_phys", cr3_phys);
}

void vmm_map_page(uint64_t *pml4, uint64_t vaddr, uint64_t paddr, uint64_t flags) {
    /* Pecah alamat virtual menjadi 4 index tabel x86_64 */
    uint64_t pml4_idx = (vaddr >> 39) & 0x1FF;
    uint64_t pdpt_idx = (vaddr >> 30) & 0x1FF;
    uint64_t pd_idx   = (vaddr >> 21) & 0x1FF;
    uint64_t pt_idx   = (vaddr >> 12) & 0x1FF;

    /* 1. Cek PDPT, buat jika belum ada */
    if (!(pml4[pml4_idx] & PAGE_PRESENT)) {
        uint64_t new_pdpt = (uint64_t)pmm_alloc_page();
        vmm_memset((void*)(new_pdpt + hhdm_offset), 0, 4096);
        pml4[pml4_idx] = new_pdpt | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }
    uint64_t *pdpt = (uint64_t*)((pml4[pml4_idx] & ~0xFFF) + hhdm_offset);

    /* 2. Cek PD, buat jika belum ada */
    if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) {
        uint64_t new_pd = (uint64_t)pmm_alloc_page();
        vmm_memset((void*)(new_pd + hhdm_offset), 0, 4096);
        pdpt[pdpt_idx] = new_pd | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }
    uint64_t *pd = (uint64_t*)((pdpt[pdpt_idx] & ~0xFFF) + hhdm_offset);

    /* 3. Cek PT, buat jika belum ada */
    if (!(pd[pd_idx] & PAGE_PRESENT)) {
        uint64_t new_pt = (uint64_t)pmm_alloc_page();
        vmm_memset((void*)(new_pt + hhdm_offset), 0, 4096);
        pd[pd_idx] = new_pt | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }
    uint64_t *pt = (uint64_t*)((pd[pd_idx] & ~0xFFF) + hhdm_offset);

    /* 4. Tulis alamat fisik asli ke Page Table (Tujuan Akhir) */
    pt[pt_idx] = (paddr & ~0xFFF) | flags;
}

uint64_t* vmm_get_active_pml4(void) {
    return current_pml4;
}
