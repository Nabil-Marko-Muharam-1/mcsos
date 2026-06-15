void load_and_run_user_program(void) {
    __asm__ volatile (
        // 1. sys_open("rahasia_user.txt")
        "mov $2, %%rax \n\t"
        "lea file_name(%%rip), %%rdi \n\t"
        "syscall \n\t"
        "mov %%rax, %%r12 \n\t"     // Simpan tiket FD yang dikembalikan kernel ke %r12

        // 2. sys_write(fd, msg_write, 28)
        "mov $1, %%rax \n\t"
        "mov %%r12, %%rdi \n\t"     // Gunakan tiket FD
        "lea msg_write(%%rip), %%rsi \n\t"
        "mov $28, %%rdx \n\t"       // Panjang string
        "syscall \n\t"

        // 3. sys_read(fd, msg_read, 28)
        "mov $0, %%rax \n\t"
        "mov %%r12, %%rdi \n\t"     // Gunakan tiket FD
        "lea msg_read(%%rip), %%rsi \n\t"
        "mov $28, %%rdx \n\t"
        "syscall \n\t"

        // 4. sys_write(1, msg_read, 28) -> Cetak ke STDOUT
        "mov $1, %%rax \n\t"
        "mov $1, %%rdi \n\t"        // Tiket FD 1 = STDOUT (Serial Monitor)
        "lea msg_read(%%rip), %%rsi \n\t"
        "mov $28, %%rdx \n\t"
        "syscall \n\t"

        // 5. sys_exit()
        "mov $60, %%rax \n\t"
        "syscall \n\t"

        "jmp end_prog \n\t"

        // Data Program Pengguna
        "file_name: .asciz \"rahasia_user.txt\" \n\t"
        "msg_write: .ascii \"[USER] Teks ini dari RAMFS!\\n\" \n\t"
        "msg_read:  .space 30 \n\t"
        "end_prog: \n\t"
        : : : "rax", "rdi", "rsi", "rdx", "rcx", "r11", "r12", "memory"
    );
}
