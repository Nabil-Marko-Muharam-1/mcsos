#ifndef CONTAINER_H
#define CONTAINER_H
#include <stdint.h>

#define MAX_CONTAINERS 4

// Struktur data ruang isolasi (Namespace)
struct container {
    int active;
    int id;                 // ID Container
    char root_path[32];     // Virtual Root FS (Jail Path)
};

void container_init();
int container_create(const char* root_path);
void container_test_simulation();

#endif
