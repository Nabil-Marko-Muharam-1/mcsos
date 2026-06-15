.global syscall_entry
.extern syscall_handler
syscall_entry:
    push %rcx
    push %r11
    push %rdi
    push %rsi
    push %rdx

    # Mengatur ulang urutan register agar sesuai dengan C Calling Convention
    # Target: syscall_handler(RAX, RDI, RSI, RDX)
    mov %rdx, %rcx  # Argumen 4 (Ukuran / Size)
    mov %rsi, %rdx  # Argumen 3 (Buffer teks)
    mov %rdi, %rsi  # Argumen 2 (Tiket FD)
    mov %rax, %rdi  # Argumen 1 (Nomor Syscall)

    call syscall_handler
    # Nilai kembalian (seperti tiket FD) akan otomatis tersimpan di %rax oleh bahasa C!

    pop %rdx
    pop %rsi
    pop %rdi
    pop %r11
    pop %rcx
    jmp *%rcx
