#ifndef SEC_H
#define SEC_H

#include <stdint.h>

#define SEC_ACCESS_READ  1
#define SEC_ACCESS_WRITE 2

// Fungsi manajemen user (Capability/ACL)
void sec_set_current_uid(uint32_t uid);
uint32_t sec_get_current_uid();
int sec_check_file_access(uint32_t file_uid, uint32_t file_perms, uint32_t requested_access);

// Fungsi Hardening untuk Syscall
int sec_validate_pointer(void *ptr, uint32_t size);

#endif
