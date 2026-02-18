#include "allocator.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

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

    int* d = calloc_(5, sizeof(int));
    printf("calloc_: d[0] = %d, d[4] = %d\n", d[0], d[4]);

    d = ralloc(d, 20 * sizeof(int));
    printf("realloc: d[0] still = %d\n", d[0]);

    int* e = ralloc(NULL, sizeof(int));
    *e = 42;
    printf("realloc(NULL): %d\n", *e);

    dealloc(a);
    dealloc(c);
    dealloc(d);
    dealloc(e);

    // edge case example
    // should not allocate because of overflow, and return NULL instead of crashing
    char *buf = alloc(SIZE_MAX);
    printf("alloc(SIZE_MAX): %s\n", buf ? "BUG" : "NULL");

    int *data = calloc_(SIZE_MAX / 2, 4);
    printf("calloc_(overflow): %s\n", data ? "BUG" : "NULL");

    return 0;
}