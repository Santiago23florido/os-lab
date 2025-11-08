/**
 * @file mem.c
 * @brief Simple dynamic memory allocator for a custom heap.
 *
 * This module provides basic memory management functionality similar to
 * malloc() and free(), implemented over a statically allocated heap.
 * 
 * The allocator maintains a singly linked list of free memory blocks,
 * automatically splitting blocks on allocation and coalescing adjacent
 * blocks on release. This helps reduce external fragmentation while keeping
 * the implementation compact and efficient.
 *
 * Key characteristics:
 *  - First-fit block selection strategy.
 *  - Block headers stored inside the heap region.
 *  - Linear-time allocation and deallocation.
 *  - Coalescence of adjacent free blocks to preserve contiguous memory.
 *
 * This implementation is intended for educational purposes, embedded systems,
 * or small operating system kernels where standard library allocators are
 * unavailable or too large.
 *
 * Usage:
 *  - Call meminit() once before performing allocations.
 *  - Use memalloc(size) to request dynamic memory.
 *  - Use memfree(ptr) to release previously allocated memory.
 *
 * Limitations:
 *  - No support for realloc() or aligned allocations.
 *  - Thread-unsafe (no synchronization primitives).
 *
 * Author: <Your Name>
 * Date:   <Date>
 */

#include "../include/mem.h"
#include <stdio.h>
#include <errno.h>

char mem[16*1024];  // real memory
const size_t heap_size = sizeof(mem);
const size_t header_size = sizeof(struct memblock);

struct memblock memblock_object = {
    .size = 0,
    .next = (struct memblock*)mem
};

void coalescence(){
    struct memblock* cursor = &memblock_object;
    struct memblock* nextcursor = cursor->next;
     while(nextcursor!= NULL){
        if ((char*)cursor + header_size + cursor->size == (char*)nextcursor){
            cursor->size=cursor->size+nextcursor->size+header_size;
            cursor->next=nextcursor->next;
            nextcursor = cursor ->next;
        }else{
        cursor = nextcursor;
        nextcursor = nextcursor->next;
        }
     }

};


void* memalloc(int requested_size){  //
    struct memblock* cursor = &memblock_object;
    struct memblock* nextcursor = cursor->next;
    if(requested_size<=0){
		printf("requested size is invalid: %d\n", requested_size);
		errno = EINVAL;
		return NULL;
    }
    size_t required_size = requested_size + header_size;
    while(nextcursor != NULL && nextcursor->size < required_size) {
        cursor = nextcursor;
        nextcursor = nextcursor->next;
    }if(nextcursor== NULL){
		errno = ENOMEM;
		return NULL;
    }if(nextcursor->size>required_size){
        void* displacement=(char*)nextcursor+required_size;
        struct memblock* generatedblock = (struct memblock*)displacement;
        generatedblock->size = nextcursor->size-required_size;
        generatedblock->next = nextcursor->next;
        cursor->next = generatedblock;
        void* allocatedmem = (char*)nextcursor + header_size;
        return allocatedmem;
    }else{
        cursor->next = nextcursor->next;
        void* allocatedmem = (char*)nextcursor + header_size;
        return allocatedmem;
    }
};

void memfree(void* usedmem){
    struct memblock* reinsert = (struct memblock*)((char*)usedmem-header_size);
    struct memblock* cursor = &memblock_object;
    struct memblock* nextcursor = cursor->next;
    while(nextcursor!= NULL&&nextcursor<reinsert){
        cursor = nextcursor;
        nextcursor = nextcursor->next;
    }
    if(nextcursor==NULL){
 		errno = ENOMEM;
        return;
    }
    cursor->next=reinsert;
    reinsert->next = nextcursor;
    reinsert->size = (char*)nextcursor-(char*)usedmem;
    coalescence();
};

void meminit(){ 
    struct memblock* cursor = &memblock_object;
    cursor = cursor->next;
    cursor->size= heap_size - header_size;
    cursor ->next = NULL;
};
