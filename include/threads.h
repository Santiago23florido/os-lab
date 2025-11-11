#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <stdint.h>
#include "coroutines.h"

#define STACK_SIZE 4096
#define URGENT_CUTOFF (-10)

#define STACKS 5

#define MAX_PRIORITY 100000000

#define THREADLIMIT (STACKS-1)

__attribute__((aligned(4096)))
typedef char my_stack_t[STACK_SIZE];

extern my_stack_t stacks[STACKS];
extern cofn new_thread_needed;
extern struct scheduler* schedulerglobal;
extern struct thread* head_ref;
struct thread{
    int id;
    coroutine_t coroutine;
    int priority;
    enum { READY, FINISHED, EXECUTING , WAITING } state;
    struct thread* nxt;
};
void kill_finished_threads();
struct scheduler{
    coroutine_t coroutine;
    struct thread * current;
};

void init_scheduler();

void scheduler();

void yield1();

void thread_init(coroutine_t coroutine,int id, int priority);

void kill_thread(struct thread* t);

int get_lowest_id_below();

struct thread* sort_threads_by_priority_links(struct thread* head);

#endif
