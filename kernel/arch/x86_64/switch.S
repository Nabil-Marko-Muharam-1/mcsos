.global switch_context
.type switch_context, @function

// void switch_context(uint64_t **old_rsp, uint64_t *new_rsp);
// Aturan ABI x86_64: parameter pertama di %rdi, parameter kedua di %rsi

switch_context:
    // 1. Simpan register 'callee-saved' milik thread lama ke stack-nya
    push %rbp
    push %rbx
    push %r12
    push %r13
    push %r14
    push %r15

    // 2. Simpan posisi ujung stack thread lama (RSP) ke dalam TCB-nya (*old_rsp)
    mov %rsp, (%rdi)

    // 3. TUKAR DIMENSI! Ganti RSP lama dengan RSP thread baru dari RSI
    mov %rsi, %rsp

    // 4. Pulihkan register thread baru dari stack barunya
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %rbx
    pop %rbp

    // 5. Kembali (return) ke tempat thread baru sebelumnya ditinggalkan
    ret

.global jump_to_userspace
jump_to_userspace:
    cli
    push $0x23
    push %rsi
    pushq $0x202
    push $0x1B
    push %rdi
    iretq

.global gdt_flush
gdt_flush:
    /* rdi = &gdtr, rsi = kernel_cs, rdx = kernel_ds, rcx = tss_sel */
    lgdt (%rdi)

    /* Reload CS dengan far return trick */
    push %rsi
    lea  .reload_cs(%rip), %rax
    push %rax
    lretq
.reload_cs:
    mov %dx, %ds
    mov %dx, %es
    mov %dx, %ss
    mov %dx, %fs
    mov %dx, %gs

    /* Load TSS */
    ltr %cx
    ret
