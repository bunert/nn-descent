#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <assert.h>
#define VERBOSE 0

typedef struct heap_el {
    double key;
    struct heap_el* parent;
    struct heap_el* left;
    struct heap_el* right;
} heap_el_t;

typedef struct heap {
    int size;
    int max_indx;
    heap_el_t** els;
} heap_t;

heap_t* heap_init(int size);
void heap_insert(heap_t* heap, heap_el_t* el);
heap_el_t* heap_extract(heap_t* heap);
bool heap_empty(heap_t* heap);
