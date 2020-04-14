#include "heap.h"

int heap_parent(int i) {
    return (i-1)/2;
}
int heap_left(int i) {
    return 2*i + 1;
}
int heap_right(int i) {
    return 2*i + 2;
}

int heap_swap(heap_t* h, int i, int j) {
    if (VERBOSE) {
        printf("Swapped %f, %f\n", h->els[i]->key, h->els[j]->key);
    }

    heap_el_t* tmp = h->els[j]; 
    h->els[j] = h->els[i];
    h->els[i] = tmp;
    return j;
}

void check_heap(heap_t* heap) {
    for (int i=0; i<heap->max_indx; i++) {
        if (VERBOSE)
            printf("comp %d,  %f, %f\n",i, heap->els[i]->key, heap->els[heap_parent(i)]->key);
        assert(heap->els[i]->key<= heap->els[heap_parent(i)]->key);
    }
}

void heap_insert(heap_t* heap, heap_el_t* el) {
    heap_t* h = heap;
     
    int indx = h->max_indx;
    h->els[indx] = el;
    while (h->els[indx]->key > h->els[heap_parent(indx)]->key) {
        // swap parent and child
        // if child has larger key than parent
        //
        indx = heap_swap(h, indx, heap_parent(indx));
    }
    
    h->max_indx++;
    check_heap(heap);
}

bool heap_empty(heap_t* heap) {
    return (heap->max_indx == 0);
}

heap_el_t* heap_extract(heap_t* heap) {
    if (VERBOSE) {
        printf("extracting...\n");
    }
    if (heap_empty(heap))
        return NULL;

    heap_el_t* max_el = heap->els[0];
    // place last element at root
    heap->els[0] = heap->els[heap->max_indx-1];
    if (VERBOSE) {
        printf("new root: %f...\n", heap->els[heap->max_indx-1]->key);
    }
    heap->max_indx--;
    
    int i = 0;
    while ((heap_left(i) < heap->max_indx && heap->els[i]->key < heap->els[heap_left(i)]->key) ||
           (heap_right(i) < heap->max_indx && heap->els[i]->key < heap->els[heap_right(i)]->key)) {
        // know: one of my children is larger
        if (heap_right(i) < heap->max_indx) { // have two children
            if (heap->els[heap_right(i)]->key < heap->els[heap_left(i)]->key)
                // left one is larger than right one
                i = heap_swap(heap, i, heap_left(i)); 
            else
                i = heap_swap(heap, i, heap_right(i)); 
        } else { // only one child, which has larger key
            i = heap_swap(heap, i, heap_left(i)); 
        }
    }
    check_heap(heap);
    return max_el;
}

heap_t* heap_init(int size) {
    heap_t* h = malloc(sizeof(heap_t));
    h->els = malloc(size * sizeof(heap_el_t*));
    h->max_indx = 0;
    return h;
}
