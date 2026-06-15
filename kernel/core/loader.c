void load_and_run_user_program(void) {
    __asm__ volatile (
        "mov $1, %%rdi \n\t"
        "lea 1f(%%rip), %%rsi \n\t"
        "syscall \n\t"
        "mov $2, %%rdi \n\t"
        "syscall \n\t"
        "jmp 2f \n\t"
        "1: .asciz \"Halo dari Ring 3! Syscall berhasil.\\n\" \n\t"
        "2: \n\t"
        : : : "rdi", "rsi", "rcx", "r11", "memory"
    );
}
