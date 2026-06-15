#include "mcsos/fs/fd.h"

// Tabel File Descriptor (Maksimal 32 file terbuka sekaligus)
static vfs_node_t *fd_table[32];

void fd_init(void) {
    for (int i = 0; i < 32; i++) fd_table[i] = NULL;
}

int fd_open(vfs_node_t *node) {
    // Cari slot kosong mulai dari index 3 (0,1,2 dicadangkan untuk STDIN, STDOUT, STDERR)
    for (int i = 3; i < 32; i++) {
        if (fd_table[i] == NULL) {
            fd_table[i] = node;
            if (node->ops->open) node->ops->open(node);
            return i; // Kembalikan angka tiket (File Descriptor)
        }
    }
    return -1; // Tabel penuh
}

uint64_t fd_write(int fd, uint8_t *buffer, uint64_t size) {
    // Jika tiketnya 1 (STDOUT), kirim langsung ke layar/serial
    if (fd == 1) {
        for (uint64_t i = 0; i < size; i++) {
            __asm__ volatile ("outb %0, %1" : : "a"(buffer[i]), "Nd"((uint16_t)0x3F8));
        }
        return size;
    }

    // Jika tiketnya 3 ke atas, tulis ke dalam RAMFS (Polimorfisme beraksi!)
    if (fd >= 3 && fd < 32 && fd_table[fd] != NULL) {
        vfs_node_t *node = fd_table[fd];
        return node->ops->write(node, 0, size, buffer); // Offset 0 (sederhana)
    }
    return 0;
}

uint64_t fd_read(int fd, uint8_t *buffer, uint64_t size) {
    if (fd >= 3 && fd < 32 && fd_table[fd] != NULL) {
        vfs_node_t *node = fd_table[fd];
        return node->ops->read(node, 0, size, buffer);
    }
    return 0;
}
