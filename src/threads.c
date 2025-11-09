#include <stdio.h>
#include <stdint.h>
#include "../include/coroutines.h"
#include "../include/threads.h"
#include "../include/mem.h"

struct scheduler* schedulerglobal;
struct thread* head_ref;

my_stack_t stacks[STACKS];


void yield1(){
    printf("inside the yield\n");  
    fflush(stdout); 
    switch_coroutine(&schedulerglobal->current->coroutine,schedulerglobal->coroutine);
}

void init_scheduler(){
    schedulerglobal = memalloc(sizeof(struct scheduler));
    head_ref = NULL;
    schedulerglobal->coroutine = init_coroutine(stacks[THREADLIMIT],STACK_SIZE,scheduler);
    schedulerglobal->current = NULL;
}


struct thread* sort_threads_by_priority_links(struct thread* head) {
    if (head == NULL || head->nxt == NULL)
        return head;

    struct thread* sorted = NULL;

    struct thread* current = head;
    while (current != NULL) {
        struct thread* next = current->nxt;

        if (sorted == NULL || current->priority > sorted->priority) {
            current->nxt = sorted;
            sorted = current;
        } else {
            struct thread* temp = sorted;
            while (temp->nxt != NULL && temp->nxt->priority >= current->priority) {
                temp = temp->nxt;
            }
            current->nxt = temp->nxt;
            temp->nxt = current;
        }

        current = next;
    }

    return sorted;
}

void scheduler(){
    printf("inside the scheduler\n");  
    fflush(stdout);
    
    for(;;) {
        if (schedulerglobal->current && schedulerglobal->current->state == EXECUTING){
            schedulerglobal->current->state = READY;
        }

        if (!head_ref) {
            continue;
        }

        head_ref = sort_threads_by_priority_links(head_ref);

        struct thread* p = head_ref;
        while (p && p->state != READY){
            p = p->nxt;
        }
        
        if (!p) {
            continue;
        }

        schedulerglobal->current = p;
        p->state = EXECUTING;

        if (p->priority > 0) p->priority -= 1;

        for (struct thread* t = head_ref; t != NULL; t = t->nxt) {
            if (t == p) continue;
            long long inc  = (long long)t->priority;
            long long next = (long long)t->priority + inc;
            t->priority = (next > MAX_PRIORITY) ? MAX_PRIORITY : (int)next;
        }

        switch_coroutine(&schedulerglobal->coroutine, schedulerglobal->current->coroutine);
    }
}


void thread_init(coroutine_t coroutine, int priority){
    struct thread* threadcreated = memalloc(sizeof(struct thread));
    threadcreated->coroutine = coroutine;
    threadcreated->priority = priority;
    threadcreated->state = READY;
    threadcreated->nxt = NULL;

    if (head_ref == NULL){
        threadcreated->id = 0;
        head_ref = threadcreated;
        return;
    }

    int ct = 1;
    struct thread* it = head_ref;   // <--- NO memalloc AQUÍ
    while (it->nxt != NULL){
        it = it->nxt;
        ct++;
    }
    threadcreated->id = ct;
    it->nxt = threadcreated;
}
