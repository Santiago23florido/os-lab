#include <stdio.h>
#include "../include/mem.h"

int main() {
    printf("Initializing heap...\n");
    meminit();

    printf("\nAllocating 100 bytes...\n");
    void* p1 = memalloc(100);
    printf("p1 = %p\n", p1);

    printf("\nAllocating 200 bytes...\n");
    void* p2 = memalloc(200);
    printf("p2 = %p\n", p2);

    printf("\nFreeing p1 (100 bytes)...\n");
    memfree(p1);

    printf("\nFreeing p2 (200 bytes)...\n");
    memfree(p2);

    printf("\nHeap should now be fully coalesced again.\n");

    // Try one large allocation to verify coalescence worked
    printf("\nAllocating 500 bytes...\n");
    void* p3 = memalloc(500);
    printf("p3 = %p\n", p3);

    printf("\nFreeing p3...\n");
    memfree(p3);

    printf("\nTest complete.\n");
    return 0;
}