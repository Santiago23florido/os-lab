#ifndef THREADS_H
#define THREADS_H

#include <stdio.h>
#include <stdint.h>
#include "coroutines.h"

#define STACK_SIZE 4096

#define STACKS 5

#define THREADLIMIT (STACKS-1)

typedef char my_stack_t[STACK_SIZE];

struct thread{
    int id;
    coroutine_t coroutine;
    int priority;
    enum { READY, WAITING, EXECUTING } state;
    struct thread* nxt;
};

struct scheduler{
    coroutine_t coroutine;
    struct thread * current;
};

void init_scheduler();

void scheduler();

void yield();

void thread_init();

struct thread* sort_threads_by_priority_links(struct thread* head);

#endif
