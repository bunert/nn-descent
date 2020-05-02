#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    // one element i in heap has
    // - id ids[i]
    // - val (distance to other node) vals[i]
    // - isnews (flag whether this node is new neighbor) isnews[i]
    // they're in flat arrays to allow future vectorization

    int size; // current number of nodes in arr
    uint32_t *ids; 
    float *vals;
    bool *isnews;
} heap_t;

int heap_create(heap_t*, int);
void heap_clear();
void heap_free(heap_t*);



void max_heapify(heap_t* h, int i);
int heap_insert_bounded(heap_t* h, uint32_t id, float dist, bool isnew, int max_neighbors);
int heap_find_by_index(heap_t*, uint32_t);
