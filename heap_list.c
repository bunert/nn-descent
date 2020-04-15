#include "heap_list.h"
#include <stdio.h>

heap_list_t* heap_list_init(int size) {
    heap_list_t* h = malloc(sizeof(heap_list_t));
    if (h == NULL)
        return NULL;

    h->heaps = calloc(size, sizeof(heap_t*));
    if (h->heaps == NULL)
        return NULL;

    h->size = size;
    h->max_indx = size;
    return h;
}

heap_t* heap_list_get(heap_list_t* hl, int id) {
// retreive heap with id (B[id])
    if (id >= hl->size)
        return NULL;

    heap_t* h = hl->heaps[id];
    if (h)
        assert(h->id == id);
    return h;
}

bool heap_list_insert(heap_list_t* hl, heap_t* heap) {
    if (heap->id >= hl->size) {
        // need to make sure that this stuff is zeroed out when expanding, otherwise we cant traverse through list of heaps as if it was an array...
        assert(false);
        // expand to fit heap->id
        heap_t** heaps = realloc(hl->heaps, heap->id*sizeof(heap_t*));
        if (heaps == NULL)
            return false;

        if (VERBOSE)
            printf("Doubled heap list size...\n");
        
        hl->size = heap->id;
        hl->heaps = heaps;
    }
    
    // insert heap into heaplist
    hl->heaps[heap->id] = heap;
    hl->max_indx = (hl->max_indx > heap->id) ? hl->max_indx : heap->id;
    return true;
}

bool heap_list_reverse(heap_list_t* src, heap_list_t* dst) {
    // reverse heap list
    
    for (int i=0; i<src->max_indx; i++) {
        if (src->heaps[i] == NULL)
            continue;

        heap_t* src_h = src->heaps[i];
        int heap_id = src_h->id;
        for (int j=0; j<src_h->max_indx; j++) {
            int node_id = src_h->els[j]->node_id;
            double key = src_h->els[j]->key;
            bool newf = src_h->els[j]->newf;
            heap_t* dst_h = heap_list_get(dst, node_id);
            
            // create new heap (adj list) for node node_id
            if (dst_h == NULL) {
                dst_h = heap_init(src_h->size, node_id);
                heap_list_insert(dst, dst_h);
            }

            // insert heap_id into heap (adj list)
            heap_el_t* hel = malloc(sizeof(heap_el_t));
            hel->node_id = heap_id;
            hel->key = key;
            hel->newf = newf;

            if (!heap_insert(dst_h, hel))
                return false;
        }
    }
}

void heap_list_print(heap_list_t* hl) {
    printf("Printing heap_list\n");
    for (int i=0; i<hl->max_indx; i++) {
        heap_t* src_h = hl->heaps[i];
        if (!src_h)
            continue;
        int heap_id = src_h->id;
        printf("%d: ", heap_id);
        for (int j=0; j<src_h->max_indx; j++) {
            int node_id = src_h->els[j]->node_id;
            bool newf = src_h->els[j]->newf;
            printf("%d (%d), ", node_id, newf);    
        } 
        printf("\n");
    }
}

bool heap_list_contained_in(heap_list_t* hl, heap_list_t* hl2) {
    // check whether hl is contained in hl2 
    for (int i=0; i<hl->max_indx; i++) {
        heap_t* src_h = hl->heaps[i];
        if (!src_h)
            continue;
        int heap_id = src_h->id;
        for (int j=0; j<src_h->max_indx; j++) {
            int node_id = src_h->els[j]->node_id;
            bool newf = src_h->els[j]->newf;
            if (heap_contains(heap_list_get(hl2, heap_id), node_id))
                if (heap_get(heap_list_get(hl2, heap_id), node_id)->newf != newf)
                    return false;
            else
                return false;
        } 
        printf("\n");
    }
    return true;

}
bool heap_list_eq(heap_list_t* hl, heap_list_t* hl2) {
    // check whether heap lists are the same (not considering keys of heap!)
    return (heap_list_contained_in(hl, hl2) && heap_list_contained_in(hl2, hl));
}
