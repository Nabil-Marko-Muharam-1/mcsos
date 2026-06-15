#ifndef RAMFS_H
#define RAMFS_H
#include "mcsos/fs/vfs.h"

void ramfs_init(void);
vfs_node_t *ramfs_create_file(const char *name);

#endif
