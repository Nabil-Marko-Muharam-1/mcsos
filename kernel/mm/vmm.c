#include <mcsos/mm/vmm.h>
#include <mcsos/mm/pmm.h>
#include <mcsos/kernel/log.h>
#include <mcsos/kernel/panic.h>

extern volatile struct {
    uint64_t id[4];
    uint64_t revision;
    uint64_t offset;
} limine_hhdm_request;

#define NX_BIT     (1ULL << 63)
#define HUGE_BIT   (1ULL << 7)

static uint64_t hhdm_offset = 0;
static uint64_t *current_pml4 = (uint64_t*)0;

static inline uint64_t read_cr3(void) {
    uint64_t val;
    __asm__ volatile("mov %%cr3, %0" : "=r"(val) : : "memory");
    return val;
}

static void vmm_memset(void *ptr, uint8_t value, uint64_t num) {
    uint8_t *p = (uint8_t *)ptr;
    for (uint64_t i = 0; i < num; i++) p[i] = value;
}

/* Split 2MB huge page menjadi 512 x 4KB pages */
static void split_huge_page(uint64_t *pd, uint64_t pd_idx) {
    uint64_t huge_entry = pd[pd_idx];
    uint64_t huge_phys  = huge_entry & ~0x1FFFFFUL; /* 2MB aligned */
    uint64_t flags      = huge_entry & 0xFFF & ~HUGE_BIT;

    uint64_t new_pt_phys = (uint64_t)pmm_alloc_page();
    uint64_t *new_pt = (uint64_t*)(new_pt_phys + hhdm_offset);

    /* Isi 512 entry PT dengan 4KB pages dari huge page */
    for (int i = 0; i < 512; i++) {
        new_pt[i] = (huge_phys + i * 0x1000) | flags;
    }

    /* Ganti PD entry dengan pointer ke PT baru */
    pd[pd_idx] = new_pt_phys | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
}

void vmm_init(void) {
    log_writeln("[M5] Memulai inisialisasi Virtual Memory Manager (VMM)...");
    hhdm_offset = limine_hhdm_request.offset;
    uint64_t cr3_phys = read_cr3() & ~0xFFF;
    current_pml4 = (uint64_t*)(cr3_phys + hhdm_offset);
    log_write("[M5] Active CR3 (PML4) physical: ");
    log_key_value_hex64("cr3_phys", cr3_phys);
}

void vmm_map_page(uint64_t *pml4, uint64_t vaddr, uint64_t paddr, uint64_t flags) {
    uint64_t pml4_idx = (vaddr >> 39) & 0x1FF;
    uint64_t pdpt_idx = (vaddr >> 30) & 0x1FF;
    uint64_t pd_idx   = (vaddr >> 21) & 0x1FF;
    uint64_t pt_idx   = (vaddr >> 12) & 0x1FF;

    /* PML4 → PDPT */
    if (!(pml4[pml4_idx] & PAGE_PRESENT)) {
        uint64_t p = (uint64_t)pmm_alloc_page();
        vmm_memset((void*)(p + hhdm_offset), 0, 4096);
        pml4[pml4_idx] = p | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }
    pml4[pml4_idx] = (pml4[pml4_idx] & ~NX_BIT) | PAGE_USER;
    uint64_t *pdpt = (uint64_t*)((pml4[pml4_idx] & ~0xFFFULL) + hhdm_offset);

    /* PDPT → PD */
    if (!(pdpt[pdpt_idx] & PAGE_PRESENT)) {
        uint64_t p = (uint64_t)pmm_alloc_page();
        vmm_memset((void*)(p + hhdm_offset), 0, 4096);
        pdpt[pdpt_idx] = p | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }
    pdpt[pdpt_idx] = (pdpt[pdpt_idx] & ~NX_BIT) | PAGE_USER;
    uint64_t *pd = (uint64_t*)((pdpt[pdpt_idx] & ~0xFFFULL) + hhdm_offset);

    /* PD → PT: cek apakah huge page, split jika perlu */
    if (pd[pd_idx] & HUGE_BIT) {
        split_huge_page(pd, pd_idx);
    } else if (!(pd[pd_idx] & PAGE_PRESENT)) {
        uint64_t p = (uint64_t)pmm_alloc_page();
        vmm_memset((void*)(p + hhdm_offset), 0, 4096);
        pd[pd_idx] = p | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
    }
    pd[pd_idx] = (pd[pd_idx] & ~NX_BIT) | PAGE_USER;
    uint64_t *pt = (uint64_t*)((pd[pd_idx] & ~0xFFFULL) + hhdm_offset);

    /* PT entry */
    pt[pt_idx] = (paddr & ~0xFFFULL) | (flags & ~NX_BIT);
}

uint64_t* vmm_get_active_pml4(void) {
    return current_pml4;
}
