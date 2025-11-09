#include <stdio.h>
#include <stdint.h>
#include "../include/coroutines.h"
#include "../include/threads.h"
#include "../include/mem.h"

struct scheduler* schedulerglobal;
struct thread* head_ref;

my_stack_t stacks[STACKS];


void yield(){
    switch_coroutine(&schedulerglobal->current->coroutine,schedulerglobal->coroutine);
}

void init_scheduler(){
    schedulerglobal = memalloc(sizeof(struct scheduler));
    schedulerglobal->coroutine = init_coroutine(stacks[THREADLIMIT],STACK_SIZE,scheduler);
    schedulerglobal->current = NULL;
}


struct thread* sort_threads_by_priority_links(struct thread* head) {
    // If the list is empty or has only one node → nothing to do
    if (head == NULL || head->nxt == NULL)
        return head;

    struct thread* sorted = NULL;  // new head of the sorted list

    // Process all nodes from the original list
    struct thread* current = head;
    while (current != NULL) {
        struct thread* next = current->nxt; // store next before breaking the link

        // Insert 'current' into the correct position in the sorted list
        if (sorted == NULL || current->priority > sorted->priority) {
            // Insert at the beginning (new highest priority)
            current->nxt = sorted;
            sorted = current;
        } else {
            // Traverse the sorted list to find where to insert
            struct thread* temp = sorted;
            while (temp->nxt != NULL && temp->nxt->priority >= current->priority) {
                temp = temp->nxt;
            }
            // Insert current after temp
            current->nxt = temp->nxt;
            temp->nxt = current;
        }

        // Move to next node in the original list
        current = next;
    }

    // 'sorted' now points to the new head of the list
    return sorted;
}

void scheduler(){
    if(schedulerglobal->current->state == EXECUTING){
        schedulerglobal->current->state = READY;
    }
    head_ref = sort_threads_by_priority_links(head_ref);
    struct thread* p = head_ref;
    while(p->state!=READY){
        p=p->nxt;
    }
    if(p->state != READY){
        return;
    }
    schedulerglobal->current = p;
    switch_coroutine(&schedulerglobal->coroutine,schedulerglobal->current->coroutine);
}

void thread_init(coroutine_t coroutine, int priority){
    struct thread* threadcreated = memalloc(sizeof(struct thread));
    struct thread* it = memalloc(sizeof(struct thread));
    threadcreated->coroutine = coroutine;
    threadcreated->priority = priority;
    threadcreated->state = READY;
    threadcreated->nxt = NULL;
    if(head_ref==NULL){
        threadcreated-> id = 0;
        head_ref= threadcreated;
        return;
    }
    int ct=1;
    it=head_ref;
    while(it->nxt!=NULL){
        it=it->nxt;
        ct=ct+1;
    }
    threadcreated->id = ct;
    it->nxt = threadcreated;
}