#include "mcsos/fs/mcsfs.h"
#include "mcsos/fs/fd.h"
#include "mcsos/mm/heap.h"

// Struct kustom untuk menggabungkan block_device dan inode ke dalam 1 pointer (internal_data)
typedef struct {
    block_device_t *dev;
    mcsfs_inode_t *inode;
} mcsfs_file_data_t;

static void print_log(const char *msg) {
    int len = 0;
    while(msg[len]) len++;
    fd_write(1, (uint8_t *)msg, len);
}

static void str_cpy(char *d, const char *s) { int i=0; while(s[i]){d[i]=s[i]; i++;} d[i]=0;}

// PERBAIKAN 1 & 2: Ubah void * ke uint8_t * dan matikan warning unused parameter
static uint64_t mcsfs_vfs_read(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer) {
    (void)node; (void)offset; (void)size; (void)buffer;
    return 0;
}

static uint64_t mcsfs_vfs_write(vfs_node_t *node, uint64_t offset, uint64_t size, uint8_t *buffer) {
    (void)offset; // Matikan warning unused parameter
    
    // PERBAIKAN 3: Ekstrak struct kustom dari internal_data
    mcsfs_file_data_t *fdata = (mcsfs_file_data_t *)node->internal_data;
    block_device_t *dev = fdata->dev;
    mcsfs_inode_t *inode = fdata->inode;
    
    uint64_t written = dev->write_blocks(dev, inode->data_block, 1, (void*)buffer);
    if (written) {
        inode->size = size;
        return size;
    }
    return 0;
}

static vfs_ops_t mcsfs_vfs_ops = {
    .read = mcsfs_vfs_read,
    .write = mcsfs_vfs_write,
    .close = NULL
};

int mcsfs_format(block_device_t *dev) {
    if (!dev) return -1;
    uint8_t buffer[512];
    for (int i = 0; i < 512; i++) buffer[i] = 0;

    mcsfs_superblock_t *sb = (mcsfs_superblock_t *)buffer;
    sb->magic = MCSFS_MAGIC;
    sb->total_blocks = dev->total_blocks;
    sb->inode_count = 16;
    sb->data_block_start = 2; 

    dev->write_blocks(dev, 0, 1, buffer);

    for (int i = 0; i < 512; i++) buffer[i] = 0;
    dev->write_blocks(dev, 1, 1, buffer);

    print_log("[MCSFS] Disk berhasil diformat!\n");
    return 0;
}

int mcsfs_mount(block_device_t *dev) {
    if (!dev) return -1;
    uint8_t buffer[512];
    dev->read_blocks(dev, 0, 1, buffer);
    mcsfs_superblock_t *sb = (mcsfs_superblock_t *)buffer;

    if (sb->magic == MCSFS_MAGIC) {
        print_log("[MCSFS] Partisi MCSFS berhasil di-mount!\n");
        return 0;
    }
    return -1;
}

vfs_node_t *mcsfs_create_file(block_device_t *dev, const char *filename) {
    uint8_t buffer[512];
    dev->read_blocks(dev, 1, 1, buffer);

    mcsfs_inode_t *inodes = (mcsfs_inode_t *)buffer;
    int free_slot = -1;

    for (int i = 0; i < 16; i++) {
        if (inodes[i].is_used == 0) {
            free_slot = i;
            break;
        }
    }

    if (free_slot == -1) return NULL;

    inodes[free_slot].is_used = 1;
    inodes[free_slot].is_dir = 0;
    inodes[free_slot].size = 0;
    inodes[free_slot].data_block = 2 + free_slot; 
    str_cpy(inodes[free_slot].filename, filename);

    dev->write_blocks(dev, 1, 1, buffer);

    print_log("[MCSFS] File dibuat: ");
    print_log(filename);
    print_log("\n");

    vfs_node_t *node = (vfs_node_t *)kmalloc(sizeof(vfs_node_t));
    mcsfs_inode_t *ram_inode = (mcsfs_inode_t *)kmalloc(sizeof(mcsfs_inode_t));
    *ram_inode = inodes[free_slot];

    // Bungkus dev dan inode ke dalam struct kustom
    mcsfs_file_data_t *fdata = (mcsfs_file_data_t *)kmalloc(sizeof(mcsfs_file_data_t));
    fdata->dev = dev;
    fdata->inode = ram_inode;

    node->ops = &mcsfs_vfs_ops;
    node->internal_data = fdata; // Hanya gunakan internal_data sesuai VFS M8

    return node;
}
