#include "vec.h"
#include <float.h>

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

// macro for swapping two heap elements
// by swapping indexes n and p in all of the three arrays (ids, vals, isnews)
// must have arrays ids, vals and isnews in scope
// see max_heapify for example usage

#define swap(n,p) { \
    uint32_t t_id; float t_val; bool t_isnew; \
    t_id = ids[p]; ids[p] = ids[n]; ids[n] = t_id; \
    t_val = vals[p]; vals[p] = vals[n]; vals[n] = t_val; \
    t_isnew = isnews[p]; isnews[p] = isnews[n]; isnews[n] = t_isnew; \
}

// always use bounded insert, datastructure is not aware of its own bounds, must be supplied
int heap_insert(heap_t*, uint32_t id, float weight, bool isnew);



int heap_create(heap_t* h, int capacity)
{
    // allocate contigious blocks for each of the three arrays
    h->size = 0;
    h->ids = malloc(sizeof(uint32_t) * capacity);
    h->vals = malloc(sizeof(float) * capacity);
    h->isnews = malloc(sizeof(bool) * capacity);
    if (!h->ids || !h->vals || !h->isnews)
        return 0;

    return 1;
}

// logically remove all elements from the heap
void heap_clear(heap_t* v) { if (v) v->size = 0; }

void max_heapify(heap_t* h, int i)
{
    // enforce max-heap property after having removed maximum
    // the node at index i has replaced the old maximum as root
    // trickle down element i recursively

    // assumes that the heap always contains '_capacity' many elements
    uint32_t* ids = h->ids;
    float* vals = h->vals;
    bool* isnews = h->isnews;

    int l = (i*2)+1, r = (i*2)+2, max;
    max = (l < h->size && h->vals[l] > h->vals[i]) ? l : i;
    max = (r < h->size && h->vals[r] > h->vals[max]) ? r : max;
    if (max != i) { //one of children is larger -> swap
        swap(i,max); // macro that swaps the two nodes by swapping the values in each of the 3 arrays
        max_heapify(h, max);
    }
}

// inserts given node while keeping the number of elements in heap <= max_neighbors
int heap_insert_bounded(heap_t* h, uint32_t id, float dist, bool isnew, int max_neighbors)
{
    if (h->size<max_neighbors) {
        return heap_insert(h, id, dist, isnew);
    } else {
        if (heap_find_by_index(h, id) >= 0)
            return 0;
        float curr_max = h->vals[0];
        if (dist > curr_max) {
            h->vals[0] = dist;
            h->isnews[0] = isnew;
            h->ids[0] = id; 
        }
        max_heapify(h, 0); 
        return 1;
    }

}


int heap_insert(heap_t* h, uint32_t id, float val, bool isnew)
{
    uint32_t* ids = h->ids;
    float* vals = h->vals;
    bool* isnews = h->isnews;

    if (heap_find_by_index(h, id) >= 0) return 0;

    // add new node, then bubble up
    ids[h->size] = id;
    vals[h->size] = val;
    isnews[h->size] = isnew;

    if (h->size != 0) {
        int n = h->size;
        int p = (n-1)/2;

        while (p >= 0 && vals[p] < vals[n]) {
            swap(n,p); // macro that swaps the two nodes by swapping the values in each of the 3 arrays
            n = p;
            p = (n-1)/2;
        }
    }

    h->size++;
    return 1;
}

int heap_find_by_index(heap_t* h, uint32_t index)
{
    int size = h->size;
    uint32_t *ids = h->ids;
    for (int i = 0; i < size; i++) {
        if (ids[i] == index) return i;
    }
    return -1;
}

void heap_free(heap_t* h) {
    if (h) {
        free(h->ids);
        free(h->isnews);
        free(h->vals);
    }
}
