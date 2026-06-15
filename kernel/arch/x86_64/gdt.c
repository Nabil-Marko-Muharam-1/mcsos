#include <stdint.h>
#include <mcsos/kernel/log.h>

typedef struct __attribute__((packed)) {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  gran;
    uint8_t  base_high;
} GdtEntry;

typedef struct __attribute__((packed)) {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  gran;
    uint8_t  base_high;
    uint32_t base_upper;
    uint32_t reserved;
} TssEntry;

typedef struct __attribute__((packed)) {
    uint16_t limit;
    uint64_t base;
} Gdtr;

typedef struct __attribute__((packed)) {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb_offset;
} Tss;

/* Taruh GDT, TSS, stack di BSS agar tidak bentrok dengan .data */
static GdtEntry gdt[7]                          __attribute__((aligned(8)));
static Tss      tss                             __attribute__((aligned(16)));
static uint8_t  kernel_stack[4096]              __attribute__((aligned(16)));
static Gdtr     gdtr;

static void gdt_set_entry(int i, uint8_t access, uint8_t gran) {
    gdt[i].limit_low = 0xFFFF;
    gdt[i].base_low  = 0;
    gdt[i].base_mid  = 0;
    gdt[i].access    = access;
    gdt[i].gran      = gran;
    gdt[i].base_high = 0;
}

extern void gdt_flush(uint64_t gdtr_addr, uint64_t kernel_cs,
                      uint64_t kernel_ds, uint64_t tss_sel);

void gdt_init(void) {
    gdt[0] = (GdtEntry){0};
    gdt_set_entry(1, 0x9A, 0x20); /* Kernel Code 64 */
    gdt_set_entry(2, 0x92, 0x00); /* Kernel Data 64 */
    gdt_set_entry(3, 0xFA, 0x20); /* User Code 64   */
    gdt_set_entry(4, 0xF2, 0x00); /* User Data 64   */

    /* TSS descriptor di slot 5 (occupies gdt[5] dan gdt[6]) */
    uint64_t tss_base  = (uint64_t)&tss;
    uint32_t tss_limit = (uint32_t)sizeof(Tss) - 1;

    TssEntry *te = (TssEntry *)&gdt[5];
    te->limit_low  = (uint16_t)(tss_limit & 0xFFFF);
    te->base_low   = (uint16_t)(tss_base & 0xFFFF);
    te->base_mid   = (uint8_t)((tss_base >> 16) & 0xFF);
    te->access     = 0x89;
    te->gran       = (uint8_t)((tss_limit >> 16) & 0x0F);
    te->base_high  = (uint8_t)((tss_base >> 24) & 0xFF);
    te->base_upper = (uint32_t)(tss_base >> 32);
    te->reserved   = 0;

    tss.rsp0        = (uint64_t)(kernel_stack + sizeof(kernel_stack));
    tss.iopb_offset = (uint16_t)sizeof(Tss);

    gdtr.limit = (uint16_t)(sizeof(gdt) - 1);
    gdtr.base  = (uint64_t)&gdt[0];

    log_key_value_hex64("[M7] TSS  addr", (uint64_t)&tss);
    log_key_value_hex64("[M7] GDT  addr", (uint64_t)&gdt[0]);

    gdt_flush((uint64_t)&gdtr, 0x08, 0x10, 0x28);
    log_writeln("[M7] GDT with ring-3 segments loaded");
}
