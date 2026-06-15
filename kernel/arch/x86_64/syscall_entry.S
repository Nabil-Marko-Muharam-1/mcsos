.global syscall_entry
.extern syscall_handler

syscall_entry:
    push %rcx
    push %r11
    push %rdi
    push %rsi
    push %rdx

    call syscall_handler

    pop %rdx
    pop %rsi
    pop %rdi
    pop %r11
    pop %rcx

    /* Kembali ke caller dengan aman menghindari GP Fault */
    jmp *%rcx
