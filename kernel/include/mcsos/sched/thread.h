#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

struct execution_context {
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t rbx;
    uint64_t rbp;
    uint64_t rip;
} __attribute__((packed));

enum thread_state {
    THREAD_READY,
    THREAD_RUNNING,
    THREAD_BLOCKED,
    THREAD_DEAD
};

struct thread {
    uint64_t rsp;
    uint64_t *stack_base;
    uint64_t id;
    enum thread_state state;
    struct thread *next;
};

// Deklarasi fungsi agar kmain.c mengenalinya
struct thread* thread_create(void (*entry_point)());
void schedule();

#endif
