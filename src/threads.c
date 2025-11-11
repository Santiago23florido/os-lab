#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include "../include/coroutines.h"
#include "../include/threads.h"
#include "../include/mem.h"
#include "../include/protect.h"

cofn new_thread_needed = NULL;
struct scheduler* schedulerglobal;
struct thread* head_ref;

my_stack_t stacks[STACKS];


void yield1(){
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
    for(;;) {

        if (new_thread_needed != NULL) {
            int index = get_lowest_id_below();
            if (index != -1) {
                mprotect(stacks[index], STACK_SIZE, PROT_READ | PROT_WRITE);
                coroutine_t new_cor = init_coroutine(stacks[index], STACK_SIZE, new_thread_needed);
                thread_init(new_cor, index, -10);
                new_thread_needed = NULL;
                mprotect(stacks[index], STACK_SIZE, PROT_NONE);
            }
        }

        if (g_sesgv == 1) {
            coroutine_t corout = schedulerglobal->current->coroutine;
            int prior = schedulerglobal->current->priority;
            int id = schedulerglobal->current->id;
            kill_thread(schedulerglobal->current);
            thread_init(corout, id, prior);
            g_sesgv = 0;
            continue;
        }

        if (schedulerglobal->current && schedulerglobal->current->state == EXECUTING) {
            schedulerglobal->current->state = READY;
        }

        if (!head_ref) continue;

        kill_finished_threads();

        struct thread* urg = NULL;
        for (struct thread* t = head_ref; t; t = t->nxt) {
            if (t->state == READY && t->priority <= -10) { urg = t; break; }
        }

        if (urg != NULL) {
            for (struct thread* t = head_ref; t; t = t->nxt) {
                if (t != urg && t->state == READY) t->state = WAITING;
            }

            if (head_ref != urg) {
                struct thread* prev = NULL;
                struct thread* cur  = head_ref;
                while (cur && cur != urg) { prev = cur; cur = cur->nxt; }
                if (cur) {
                    if (prev) prev->nxt = cur->nxt;
                    cur->nxt = head_ref;
                    head_ref = cur;
                }
            }

            struct thread* p = urg;
            schedulerglobal->current = p;
            p->state = EXECUTING;

            if (p->priority > -10 && p->priority > 0) p->priority -= 1;

            for (struct thread* t = head_ref; t; t = t->nxt) {
                if (t == p) continue;
                if (t->priority > -10) {
                    long long inc  = (long long)t->priority;
                    long long next = (long long)t->priority + inc;
                    t->priority = (next > MAX_PRIORITY) ? MAX_PRIORITY : (int)next;
                }
            }

            mprotect(stacks[p->id], STACK_SIZE, PROT_READ | PROT_WRITE);
            mprotect(stacks[THREADLIMIT], STACK_SIZE, PROT_READ | PROT_WRITE);
            switch_coroutine(&schedulerglobal->coroutine, p->coroutine);
            mprotect(stacks[p->id], STACK_SIZE, PROT_NONE);

            if (p->state == FINISHED) {
                for (struct thread* t = head_ref; t; t = t->nxt) {
                    if (t->state == WAITING) t->state = READY;
                }
            }
            continue;
        }

        for (struct thread* t = head_ref; t; t = t->nxt) {
            if (t->state == WAITING) t->state = READY;
        }

        head_ref = sort_threads_by_priority_links(head_ref);

        struct thread* p = head_ref;
        while (p && p->state != READY) p = p->nxt;
        if (!p) continue;

        schedulerglobal->current = p;
        p->state = EXECUTING;

        if (p->priority > 0) p->priority -= 1;

        for (struct thread* t = head_ref; t != NULL; t = t->nxt) {
            if (t == p) continue;
            if (t->priority > -10) {
                long long inc  = (long long)t->priority;
                long long next = (long long)t->priority + inc;
                t->priority = (next > MAX_PRIORITY) ? MAX_PRIORITY : (int)next;
            }
        }

        mprotect(stacks[p->id], STACK_SIZE, PROT_READ | PROT_WRITE);
        mprotect(stacks[THREADLIMIT], STACK_SIZE, PROT_READ | PROT_WRITE);
        switch_coroutine(&schedulerglobal->coroutine, p->coroutine);
        mprotect(stacks[p->id], STACK_SIZE, PROT_NONE);
    }
}



void kill_thread(struct thread* t) {

    if (t==schedulerglobal->current){
        struct thread* nxt= schedulerglobal->current->nxt;
        schedulerglobal->current=nxt;
    }
    if (head_ref == t) head_ref = t->nxt;
    else {
        struct thread *p = head_ref;
        while (p && p->nxt != t) p = p->nxt;
        if (p) p->nxt = t->nxt;
    }
}

void kill_finished_threads() {
    struct thread* prev = NULL;
    struct thread* t = head_ref;
    while (t) {
        if (t->state == FINISHED && t != schedulerglobal->current) {
            struct thread* dead = t;
            t = t->nxt;
            if (prev) prev->nxt = dead->nxt;
            else head_ref = dead->nxt;
            kill_thread(dead);
            continue;
        }
        prev = t;
        t = t->nxt;
    }
}

void thread_init(coroutine_t coroutine, int id, int priority){
    struct thread *it, *threadcreated;

    threadcreated = memalloc(sizeof(struct thread));
    threadcreated->coroutine = coroutine;
    threadcreated->priority = priority;
    threadcreated->state = READY;
    threadcreated->id = id;
    threadcreated->nxt = NULL;

    if (head_ref == NULL){
        head_ref = threadcreated;
        return;
    }

    it = head_ref;
    while (it->nxt != NULL) it = it->nxt;
    it->nxt = threadcreated;
}


int get_lowest_id_below(){
    int limit = THREADLIMIT - 1;
    
    for(int id = 0; id < limit; id++){
        int used = 0;
        struct thread *it = head_ref;
        while(it){
            if(it->id == id){
                used = 1;
                break;
            }
            it = it->nxt;
        }
        if(!used) return id;
    }
    return -1;
}