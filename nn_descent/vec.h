#pragma once
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    // element of a heap

    int id; // node id of corresponding point
    float val; // dissimilarity to heap-owner point
    bool new;
} node_t;


// vector of dimension _capacity. Used for the heaps
typedef struct {
    int size; // current number of nodes in arr
    int _capacity; // maximum number of nodes in arr (K)
    float min; // smallest dissimilarity of node in heap
    node_t* arr;
} vec_t;

int vec_create(vec_t*, int);
void vec_clear();
void vec_free(vec_t*);

int vec_insert(vec_t*, node_t*);

int heap_insert(vec_t*, node_t*);
int heap_union(vec_t*, vec_t*);
int heap_find_by_index(vec_t*, int);

