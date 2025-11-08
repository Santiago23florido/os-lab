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
#include <stddef.h>

struct memblock {
    size_t size;
    struct memblock* next;
};

/* Only declarations here — no memory allocation */
extern char mem[16*1024];
extern struct memblock memblock_object;
extern const size_t header_size;
extern const size_t heap_size;

void meminit();
void* memalloc(int requested_size);
void memfree(void* usedmem);
void coalescence();

#endif

