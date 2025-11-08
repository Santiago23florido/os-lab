#include "../include/mem.h"
#include <stdio.h>



void* memalloc(int size){

};

void meminit(){ //Mem initial state
    struct memblock* cursor = memblock_object.next; // cursor in heap init
    cursor -> size = heap_size - header_size;
    cursor -> next = NULL;
};
