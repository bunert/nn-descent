#pragma once
#include "vec.h"
#include <stdbool.h>

// TODO: maybe use bit flags for bool argument in sample_neighbors?

typedef struct {
    float** values; // be sure to cast to float or double types
    int size; // number of points in the dataset
    int dim;  
} dataset_t;

int make_test_data(dataset_t*, int, int);
heap_t* heap_list_create(int, int);
void heap_list_free(heap_t*, int);
heap_t* nn_descent(dataset_t, float(*)(float*, float*, int), int, float, float);
int sample_reverse_union(heap_t* new, heap_t* old, heap_t* B, int k, int N);
int update_nn(heap_t*, int, float);
int reverse_heap_list(heap_t*, heap_t*, int);

