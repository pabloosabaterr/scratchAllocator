#include "allocator.h"
#include <stdio.h>

/**
 * Tests
 */
int main(){
    // Example
    int* a = alloc(10 * sizeof(int));
    for(int i = 0; i < 10; i++) a[i] = i;
    printf("alloc: a[5] = %d\n", a[5]);
    int* c = alloc(8 * sizeof(int));
    printf("reuse: c == a? %s\n", (void*)c == (void*)a ? "yes" : "no");

    int* d = calloc(5, sizeof(int));
    printf("calloc: d[0] = %d, d[4] = %d\n", d[0], d[4]);

    d = realloc(d, 20 * sizeof(int));
    printf("realloc: d[0] still = %d\n", d[0]);

    int* e = realloc(NULL, sizeof(int));
    *e = 42;
    printf("realloc(NULL): %d\n", *e);

    free(a);
    free(c);
    free(d);
    free(e);
    return 0;
}