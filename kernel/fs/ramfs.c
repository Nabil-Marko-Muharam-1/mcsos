#include "mcsos/fs/ramfs.h"
#include "mcsos/mm/heap.h"

// Fungsi utilitas memori internal untuk menyalin data
static void *internal_memcpy(void *dest, const void *src, uint64_t n) {
    uint8_t *d = dest;
    const uint8_t *s = src;
    for (uint64_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

// Membaca data dari RAM ke buffer user
static uint64_t ramfs_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer) {
    if (offset >= node->length) return 0;
    
    uint64_t bytes_to_read = size;
    if (offset + size > node->length) {
        bytes_to_read = node->length - offset;
    }
    
    uint8_t *file_data = (uint8_t *)node->internal_data;
    internal_memcpy(buffer, file_data + offset, bytes_to_read);
    return bytes_to_read;
}

// Menulis data dari buffer user ke dalam RAM
static uint64_t ramfs_write(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer) {
    uint64_t max_size = 4096; // Batas statis 4 KB per file untuk M8
    if (offset >= max_size) return 0;
    
    uint64_t bytes_to_write = size;
    if (offset + size > max_size) {
        bytes_to_write = max_size - offset;
    }
    
    uint8_t *file_data = (uint8_t *)node->internal_data;
    internal_memcpy(file_data + offset, buffer, bytes_to_write);

    if (offset + bytes_to_write > node->length) {
        node->length = offset + bytes_to_write;
    }
    return bytes_to_write;
}

// Operasi buka dan tutup (kosong untuk RAMFS sederhana)
static void ramfs_open(vfs_node_t *node) { (void)node; }
static void ramfs_close(vfs_node_t *node) { (void)node; }

// Tabel Polimorfisme (Function Pointers)
static vfs_ops_t ramfs_ops = {
    .read = ramfs_read,
    .write = ramfs_write,
    .open = ramfs_open,
    .close = ramfs_close
};

// Fungsi untuk membuat node file baru di RAM
vfs_node_t *ramfs_create_file(const char *name) {
    vfs_node_t *new_node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
    if (!new_node) return NULL;

    // Menyalin nama file
    int i = 0;
    while (name[i] && i < 127) {
        new_node->name[i] = name[i];
        i++;
    }
    new_node->name[i] = '\0';

    new_node->type = FS_FILE;
    new_node->length = 0;
    new_node->ops = &ramfs_ops;            // Tautkan tabel operasi!
    new_node->internal_data = kmalloc(4096); // Pinjam 4 KB dari Heap
    new_node->next = NULL;

    return new_node;
}

void ramfs_init(void) {
    // Akan digunakan nanti jika kita membuat sistem direktori berlapis (Tree)
}
