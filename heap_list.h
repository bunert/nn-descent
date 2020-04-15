#include "heap.h"

typedef struct heap_list {
    int size;
    int max_indx;
    heap_t** heaps;
} heap_list_t;

heap_list_t* heap_list_init(int size);
heap_t* heap_list_get(heap_list_t* hl, int id);
bool heap_list_insert(heap_list_t* hl, heap_t* heap);
bool heap_list_reverse(heap_list_t* src, heap_list_t* dst);
void heap_list_print(heap_list_t* hl);
bool heap_list_eq(heap_list_t* hl, heap_list_t* hl2);

