#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>
#include <assert.h>
#define VERBOSE 0

typedef struct heap_el {
    double key;
    int node_id;
    bool newf;
} heap_el_t;

typedef struct heap {
    int size;
    int max_indx;
    int id;
    heap_el_t** els;
} heap_t;

heap_t* heap_init(int size, int id);
bool heap_insert(heap_t* heap, heap_el_t* el);
heap_el_t* heap_extract(heap_t* heap);
heap_el_t* heap_peek(heap_t* heap);
bool heap_empty(heap_t* heap);
bool heap_contains(heap_t* heap, int id);
heap_el_t* heap_get(heap_t* heap, int id);
