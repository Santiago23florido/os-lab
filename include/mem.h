/**
 * @file mem.h
 * @brief Memory management subsystem for MiniOS.
 *
 * This module provides a simple heap manager for MiniOS, allowing
 * dynamic memory allocation and release. It simulates low-level
 * heap behavior similar to malloc/free in a controlled environment.
 *
 * @author Santiago Florido
 * @date 2025
 */

#ifndef MEM_H
#define MEM_H
#include <stddef.h>  // For size_t


static char mem[16*1024]; //HEAP declaration


struct memblock{                  //Struct of free heap memory 
    size_t size;
    struct memblock* next;
};

struct memblock memblock_object = {     //HEAP global header
    .size = 0,
    .next = (struct memblock*)mem       
};

const size_t header_size = sizeof(struct memblock);
const size_t heap_size = sizeof(mem);
void* memalloc(int size);
void meminit();

#endif /* MEM_H */
