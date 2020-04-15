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

bool heap_insert(heap_t* heap, heap_el_t* el) {
    heap_t* h = heap;
    if (h->max_indx == h->size) {
        heap_el_t** els = realloc(h->els, h->size*2*sizeof(heap_el_t));
        if (els == NULL)
            return false;

        if (VERBOSE)
            printf("Doubled heap size...\n");
        
        h->size *= 2;
        h->els = els;
    }
     
    int indx = h->max_indx;
    h->els[indx] = el;

    // swap parent and child
    // if child has larger key than parent
    //
    while (h->els[indx]->key > h->els[heap_parent(indx)]->key)
        indx = heap_swap(h, indx, heap_parent(indx)); 
    
    h->max_indx++;
    check_heap(heap);
    return true;
}

bool heap_empty(heap_t* heap) {
    return (heap->max_indx == 0);
}

heap_el_t* heap_peek(heap_t* heap) {
    if (heap_empty(heap))
        return NULL;

    return heap->els[0];
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
    double el_k = heap->els[i]->key;
    double left_k = heap->els[heap_left(i)]->key;
    double right_k = heap->els[heap_right(i)]->key;

    while ((heap_left(i) < heap->max_indx && el_k < left_k) ||
           (heap_right(i) < heap->max_indx && el_k < right_k)) {
        // know: one of my children is larger
        if (heap_right(i) < heap->max_indx) { // have two children
            if (left_k > right_k)
                // left one is larger than right one
                i = heap_swap(heap, i, heap_left(i)); 
            else
                i = heap_swap(heap, i, heap_right(i)); 
        } else { // only one child, which has larger key
            i = heap_swap(heap, i, heap_left(i)); 
        }

    el_k = heap->els[i]->key;
    if (heap_left(i) < heap->max_indx)
        left_k = heap->els[heap_left(i)]->key;
    if (heap_right(i) < heap->max_indx)
        right_k = heap->els[heap_right(i)]->key;
    }
    check_heap(heap);
    return max_el;
}

heap_t* heap_init(int size, int id) {
    heap_t* h = malloc(sizeof(heap_t));
    if (h == NULL)
        return NULL;

    h->els = malloc(size * sizeof(heap_el_t*));
    if (h->els == NULL)
        return NULL;

    h->max_indx = 0;
    h->size = size;
    h->id = id;
    return h;
}

bool heap_contains(heap_t* heap, int id) {
    if (heap == NULL)
        return false;
    for (int i=0; i<heap->max_indx; i++)
        if (heap->els[i]->node_id == id)
            return true;
    return false;
}
