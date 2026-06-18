#ifndef MCSFS_H
#define MCSFS_H

#include <stdint.h>
#include "mcsos/dev/block.h"
#include "mcsos/fs/vfs.h"

#define MCSFS_MAGIC 0x4D435346

typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t inode_count;
    uint32_t data_block_start;
} mcsfs_superblock_t;

typedef struct {
    uint32_t is_used;
    uint32_t is_dir;
    uint32_t size;
    uint32_t data_block;
    char filename[16];
} mcsfs_inode_t;

int mcsfs_format(block_device_t *dev);
int mcsfs_mount(block_device_t *dev);
vfs_node_t *mcsfs_create_file(block_device_t *dev, const char *filename);

#endif
