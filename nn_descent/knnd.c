#include "knnd.h"
#include "vec.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

heap_t* heap_list_create(int size, int k)
{
    // create list of 'size' many max heaps

    heap_t* hl = malloc(sizeof(heap_t) * size);
    hl->size = size;
    if (!hl) return NULL;

    for (int i = 0; i < size; i++) {
        if (!heap_create(&hl[i], k)) return NULL;
    }

    return hl;
}

void heap_list_clear(heap_t* hl, int size) {
    // logically clears/empyies heap list
    for (int i = 0; i < size; i++) {
        heap_clear(&hl[i]);
    }
}

void heap_list_free(heap_t* hl, int size)
{
    for (int i = 0; i < size; i++) {
        heap_free(&hl[i]);
    }

    free(hl);
}

int sum(heap_t* B, int N) {
    int sum=0;
    // this function samples, reverses and does the union all at once
    for (int i=0; i<N; i++) {
// printf("i: %d, sum: %d\n", i, (B[i].fwd_new + B[i].fwd_old));
        // assert(B[i].fwd_new + B[i].fwd_old == 20);
        sum += B[i].fwd_new + B[i].fwd_old + B[i].rev_new + B[i].rev_old;

        /*for (int j=0; j<heap.size; j++) {
            
            // here we insert connections using some random weight in [0,1]
            // this corresponds to using the max_candidates-many neighbors with
            // the lowest (random) weight, which should be equivalent to sampling
            // max_candidates-many entries u.a.r. from the neighors of each node
            bool isnew = heap.isnews[j]; 
            uint32_t v = heap.ids[j];
            heap_t heap_v = B[v];
        }*/
    }
    return sum;
}


int nn_update(heap_t* B, heap_t* h, uint32_t id, float dist)
{
    // insert node into maxheap h if it is more similar than the current root

    if (dist >= h->vals[0] || heap_find_by_index(h, id) >= 0) return 0;

    heap_t *R = &B[h->ids[0]];
    if (h->ids[0] != id) {
        if (h->isnews[0]) {

            R->rev_new--;
            B[id].rev_new++;
        } else {
            // to be replaced is not new
            R->rev_old--;    
            B[id].rev_new++;

            // replacing forward old with
            // forward new
            h->fwd_old--;
            h->fwd_new++;
        }
    }


    h->vals[0] = dist;
    h->isnews[0] = true;
    h->ids[0] = id;

    max_heapify(h, 0);

    return 1;
}

heap_t* nn_descent(dataset_t data, float(*metric)(float*, float*, int), int k, float rho, float delta)
{
    // Implementation of Algorithm 2: NNDescentFull from the publication

    // metric: function returning disimilarity between two vectors of given dimension
    // rho: sample rate in (0,1] (2.5 Sampling)
    // delta: precision parameter, terminate if less than delta*K*N updates in an iteration
    int max_candidates = 50;

    if (k >= data.size) {
        printf("error: neighborhood size must be less than dataset size\n");
        return NULL;
    }

    // create lists with the maxheaps for each datapoint
    heap_t* B     = heap_list_create(data.size, k);
    heap_t* old   = heap_list_create(data.size, max_candidates);
    heap_t* new   = heap_list_create(data.size, max_candidates);

    if (!B || !old || !new) {
        printf("error: failed to allocate one or more heap lists\n");
        if (B) heap_list_free(B, data.size);
        if (old) heap_list_free(old, data.size);
        if (new) heap_list_free(new, data.size);
        return NULL;
    }

    // initialize heap list B
    for (int i = 0; i < data.size; i++) {
        // sample 
        for (int j = 0; j < k;) {
            uint32_t id = (int)rand() % data.size;
            if (heap_insert_bounded(&B[i], id, FLT_MAX, true, k) == 1) {
                j++;    
                B[id].rev_new++;
                B[i].fwd_new++;
            }
        }
    }
    //printf("sum: %d, expected: %d\n", sum(B, data.size),data.size*20*2);
    // assert(sum(B, data.size)==(int)data.size*20*2);

    int c; // number of updates in this iteration
    int stop_iter = delta * data.size * k; // terminate if less than this many changes made

    do {
        heap_list_clear(old, data.size);
        heap_list_clear(new, data.size);

        sample_reverse_union(new, old, B, max_candidates, data.size);

        c = 0;
        for (int v = 0; v < data.size; v++) {
            for (int i = 0; i < new[v].size; i++) {

                uint32_t u = new[v].ids[i];

                int index = heap_find_by_index(&B[u], v);
                if (index >= 0) {
                    B[u].isnews[index] = false;
                    B[u].fwd_new--;
                    B[u].fwd_old++;

                    B[v].rev_new--;
                    B[v].rev_old++;
                }

                index = heap_find_by_index(&B[v], u);
                if (index >= 0) {
                    B[v].isnews[index] = false;
                    B[v].fwd_new--;
                    B[v].fwd_old++;

                    B[u].rev_new--;
                    B[u].rev_old++;
                }
    // printf("sum: %d, expected: %d\n", sum(B, data.size),data.size*20*2);
    // assert(sum(B, data.size)==data.size*20*2);

                for (int k=0; k<2; k++) {
                    heap_t* heap_list = (k==0) ? new : old;

                    for (int j = 0; j < heap_list[v].size; j++) {
                        if (i == j) continue;

                        uint32_t u1_id, u2_id;
                        u1_id = new[v].ids[i];
                        u2_id = heap_list[v].ids[j];

                        if (u1_id <= u2_id) continue;

                        float l = metric(data.values[u1_id], data.values[u2_id], data.dim);

    // printf("sum: %d, expected: %d\n", sum(B, data.size),data.size*20*2);
    // assert(sum(B, data.size)==data.size*20*2);
                        c += nn_update(B, &B[u1_id], u2_id, l);
                        c += nn_update(B, &B[u2_id], u1_id, l);
    // printf("sum: %d, expected: %d\n", sum(B, data.size),data.size*20*2);
    // assert(sum(B, data.size)==data.size*20*2);
                    }
                }    
            }
        }
       // printf("iteration complete: %d / %d\n", c, stop_iter);
    } while (c >= stop_iter);
    printf("NNDescent finished \n");
    //printf("done, cleaning up...\n");

    heap_list_free(old, data.size);
    heap_list_free(new, data.size);

    return B;
}


int sample_reverse_union(heap_t* new, heap_t* old, heap_t* B, int max_candidates, int N) {
    // this function samples, reverses and does the union all at once
    for (int i=0; i<N; i++) {
        heap_t heap = B[i];
        int u = i;

        for (int j=0; j<heap.size; j++) {
            // here we insert connections using some random weight in [0,1]
            // this corresponds to using the max_candidates-many neighbors with
            // the lowest (random) weight, which should be equivalent to sampling
            // max_candidates-many entries u.a.r. from the neighors of each node
            bool isnew = heap.isnews[j]; 
            uint32_t v = heap.ids[j];
            heap_t heap_v = B[v];

            float rand_weight = rand()/(float)RAND_MAX;

            // depending on flag select new or old heaplist
            heap_t* heap_list = (isnew) ? new : old;
            float pr_u = (isnew) ? (float)2*max_candidates/(heap.fwd_new + heap.rev_new) :  (float) 2*max_candidates/(heap.fwd_old + heap.rev_old);
            float pr_v = (isnew) ? (float)2*max_candidates/(heap_v.fwd_new + heap_v.rev_new) :  (float)2*max_candidates/(heap_v.fwd_old + heap_v.rev_old);
 
            if (pr_u > rand_weight)
                heap_insert_bounded(&heap_list[u], v, rand_weight, isnew, max_candidates);

            if (pr_v > rand_weight)
                heap_insert_bounded(&heap_list[v], u, rand_weight, isnew, max_candidates);
                // reverse connection (add to B[v] if rand_weight amongst the K smallest...)
        }
    }
    return 0;
}

