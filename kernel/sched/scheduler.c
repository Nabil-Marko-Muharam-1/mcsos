#include <mcsos/sched/thread.h>
#include <mcsos/mm/heap.h>
#include <stddef.h>

extern void switch_context(uint64_t **old_rsp, uint64_t *new_rsp);

struct thread *current_thread = NULL;
struct thread *ready_queue = NULL;
uint64_t next_tid = 1;

#define STACK_SIZE 4096

void enqueue_thread(struct thread *t) {
    if (ready_queue == NULL) {
        ready_queue = t;
        t->next = NULL;
    } else {
        struct thread *curr = ready_queue;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = t;
        t->next = NULL;
    }
}

struct thread* thread_create(void (*entry_point)()) {
    struct thread *t = (struct thread*) kmalloc(sizeof(struct thread));
    if (!t) return NULL;

    t->stack_base = (uint64_t*) kmalloc(STACK_SIZE);
    if (!t->stack_base) return NULL;

    t->id = next_tid++;
    t->state = THREAD_READY;
    t->next = NULL;

    uint64_t *stack_top = (uint64_t*)((uint64_t)t->stack_base + STACK_SIZE);
    
    stack_top--;
    *stack_top = (uint64_t) entry_point;
    
    for (int i = 0; i < 6; i++) {
        stack_top--;
        *stack_top = 0; 
    }
    
    t->rsp = (uint64_t) stack_top;

    enqueue_thread(t);
    return t;
}

void schedule() {
    if (ready_queue == NULL) return;

    struct thread *prev = current_thread;
    struct thread *next = ready_queue;

    ready_queue = ready_queue->next;

    if (prev != NULL && prev->state == THREAD_RUNNING) {
        prev->state = THREAD_READY;
        enqueue_thread(prev);
    }

    next->state = THREAD_RUNNING;
    current_thread = next;

    // Perbaikan pointer agar Clang tidak protes
    static uint64_t dummy_rsp;
    uint64_t *dummy_rsp_ptr = &dummy_rsp;
    uint64_t **old_rsp_ptr;
    
    if (prev != NULL) {
        // Casting paksa ke pointer-to-pointer
        old_rsp_ptr = (uint64_t **)&(prev->rsp);
    } else {
        old_rsp_ptr = &dummy_rsp_ptr;
    }

    switch_context(old_rsp_ptr, (uint64_t *)next->rsp);
}
