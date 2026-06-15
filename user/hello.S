.global _start
.text
_start:
    mov $1, %rax          /* syscall number = 1 (print) */
    lea message(%rip), %rdi  /* arg1 = alamat string */
    xor %rsi, %rsi
    syscall

    mov $2, %rax          /* syscall number = 2 (exit) */
    xor %rdi, %rdi
    syscall

    /* Jika syscall exit tidak halt, loop di sini */
    jmp .
.data
message:
    .asciz "Halo dari Ring 3! Syscall berhasil.\n"
