#ifndef FD_H
#define FD_H
#include "mcsos/fs/vfs.h"

void fd_init(void);
int fd_open(vfs_node_t *node);
uint64_t fd_write(int fd, uint8_t *buffer, uint64_t size);
uint64_t fd_read(int fd, uint8_t *buffer, uint64_t size);

#endif
