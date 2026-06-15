#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>

// Menandakan apakah node ini adalah file biasa atau direktori
typedef enum {
    FS_FILE = 1,
    FS_DIRECTORY = 2,
    FS_MOUNTPOINT = 3
} vfs_node_type_t;

struct vfs_node; // Forward declaration

// VFS Interface: Kumpulan function pointer untuk operasi file standar
typedef struct {
    uint64_t (*read)(struct vfs_node *node, uint64_t offset, uint64_t size, uint8_t *buffer);
    uint64_t (*write)(struct vfs_node *node, uint64_t offset, uint64_t size, uint8_t *buffer);
    void (*open)(struct vfs_node *node);
    void (*close)(struct vfs_node *node);
} vfs_ops_t;

// VFS Node: Representasi satu file atau direktori di dalam OS
typedef struct vfs_node {
    char name[128];           // Nama file (maksimal 128 karakter)
    vfs_node_type_t type;     // Tipe (File/Directory)
    uint64_t length;          // Ukuran file dalam byte
    vfs_ops_t *ops;           // Pointer ke tabel operasi (akan diisi oleh RAMFS nanti)
    void *internal_data;      // Pointer sakti untuk menyimpan isi data mentah file
    struct vfs_node *next;    // Linked list untuk menautkan file di dalam direktori
} vfs_node_t;

#endif
