#pragma once
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    // element of a heap

    // as float is 32 bit and on my computer int is aswell
    // reorder should not matter
    int id; // node id of corresponding point
    float val; // dissimilarity to heap-owner point
    bool new;
} node_t;


// vector of dimension _capacity. Used for the heaps
// not a datapoint
typedef struct {
    node_t* arr;
    int size; // current number of nodes in arr
    int _capacity; // maximum number of nodes in arr (K)

} vec_t;

int vec_create(vec_t*, int);
void vec_clear(vec_t* v);
void vec_free(vec_t*);

int vec_insert(vec_t*, node_t*);

int heap_insert(vec_t*, node_t*);
int heap_union(vec_t*, vec_t*);
int heap_find_by_index(vec_t*, int);

