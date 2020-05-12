#include "knnd.h"
#include "vec.h"
#include "bruteforce.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

vec_t* vec_list_create(int size, int k)
{
    vec_t* vl = malloc(sizeof(vec_t) * size + sizeof(uint32_t) * k * size);
    for (int i=0; i<size; i++) {
        vl[i].size = 0;
        vl[i].ids = (uint32_t*) ((size_t)vl + sizeof(vec_t)*size + i*sizeof(uint32_t)*k);
    }
    return vl;
}


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

void vec_list_clear(vec_t* hl, int size) {
    // logically clears/empyies heap list
    for (int i = 0; i < size; i++) {
        hl[i].size = 0;
    }
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






int nn_update(heap_t* B, heap_t* h, uint32_t id, float dist)
{
    // insert node into maxheap h if it is more similar than the current root

    if (dist >= h->vals[0] || heap_find_by_index(h, id) >= 0) return 0;

    heap_t *R = &B[h->ids[0]];
    if (h->isnews[0]) {
        // if old max element in heap was already flagged new, the outward new count on h stays the same
        // we need to remove the incoming (reverse) new count on the element R it was previously pointing to
        // and increase it on B[id] where it is now pointing
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
    vec_t* old   = vec_list_create(data.size, max_candidates);
    vec_t* new   = vec_list_create(data.size, max_candidates);

    if (!B || !old || !new) {
        printf("error: failed to allocate one or more heap lists\n");
        if (B) heap_list_free(B, data.size);
        if (old) free(old);
        if (new) free(new);
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

    int c; // number of updates in this iteration
    int stop_iter = delta * data.size * k; // terminate if less than this many changes made

    const int UPD_MAX_SIZE = 8*16384;
    update_t updates;
    updates.size=0;
    updates.u = malloc(data.size*data.size*sizeof(uint32_t));
    updates.v = malloc(data.size*data.size*sizeof(uint32_t));
    updates.dist = malloc(data.size*data.size*sizeof(float));

    do {
        vec_list_clear(old, data.size);
        vec_list_clear(new, data.size);

        sample_reverse_union(new, old, B, max_candidates, data.size);

        c = 0;
        for (int v = 0; v < data.size; v++) {
            // brute force algorithm to solve KNN:
            // for new[v] x new[v]
            // for new[v] x old[v]
            nn_brute_force(metric, data, &updates, &new[v], &new[v]);
            nn_brute_force(metric, data, &updates, &new[v], &old[v]);

            // we will create at most m_c*m_c*2 updates in the next iteration
            // if theres enough space we do not need to perform the updates yet
            if (UPD_MAX_SIZE-updates.size < max_candidates*max_candidates*2) {
                for (int i=0; i<updates.size; i++) {
                    c += nn_update(B, &B[updates.u[i]], updates.v[i] , updates.dist[i]);
                    c += nn_update(B, &B[updates.v[i]], updates.u[i] , updates.dist[i]);
                }
                updates.size=0;
            }
            // N * max_c * 2 * max_c
        }
        for (int i=0; i<updates.size; i++) {
            c += nn_update(B, &B[updates.u[i]], updates.v[i] , updates.dist[i]);
            c += nn_update(B, &B[updates.v[i]], updates.u[i] , updates.dist[i]);
        }
        updates.size=0;
       // printf("iteration complete: %d / %d\n", c, stop_iter);
    } while (c >= stop_iter);
// assert(validate_connection_counters(B, data.size)==data.size*k*2);

    printf("NNDescent finished \n");

    //printf("done, cleaning up...\n");

    free(old);
    free(new);
    free(updates.u);
    free(updates.v);
    free(updates.dist);

    return B;
}



int sample_reverse_union(vec_t* new, vec_t* old, heap_t* B, int max_candidates, int N) {
    // this function samples, reverses and does the union all at once

    for (int i=0; i<N; i++) {
        heap_t heap = B[i];
        int u = i;

        for (int j=0; j<heap.size; j++) {
            bool isnew = heap.isnews[j];
            uint32_t v = heap.ids[j];
            heap_t heap_v = B[v];

            // depending on flag select new or old heaplist
            vec_t* heap_list = (isnew) ? new : old;

            // pr_u is the number of elements we can sample from for heap_list[u]
            // same story for v
            int pr_u = (isnew) ? (heap.fwd_new + heap.rev_new) :  (heap.fwd_old + heap.rev_old);
            int pr_v = (isnew) ? (heap_v.fwd_new + heap_v.rev_new) :  (heap_v.fwd_old + heap_v.rev_old);


            // corresponds to sampling with pr max_candidates/pr_u
            if (rand() % pr_u < max_candidates) {
                vec_insert_bounded(&heap_list[u], v, max_candidates);

                // if we sampled a new connection
                // we can now
                // 1. change the flag
                // 2. update our counters
                if (isnew) {
                    B[u].isnews[j] = false;
                    // by flagging B[u][v] = false
                    // we have one fewer flagged u -> v
                    // and one more not-flagged u->v
                    B[u].fwd_new--;
                    B[u].fwd_old++;

                    // by flagging B[u][v] = false
                    // we have one fewer flagged v -> u
                    // and one more not-flagged v->u
                    B[v].rev_new--;
                    B[v].rev_old++;
                }
            }

            if (rand() % pr_v < max_candidates) {
                vec_insert_bounded(&heap_list[v], u, max_candidates);

                int index = heap_find_by_index(&B[v], u);
                if (index >= 0 && B[v].isnews[index]) {
                    // same story with the flags as above
                    B[v].isnews[index] = false;
                    B[v].fwd_new--;
                    B[v].fwd_old++;

                    B[u].rev_new--;
                    B[u].rev_old++;
                }
            }
        }
    }
    return 0;
}

int validate_connection_counters(heap_t* B, int N) {
    // this checks whether all the counters are what they're supposed to be
    // obviously this is for testing and shouldnt be executed in "production" code
    int sum=0;
    int *rev_old = calloc(N,sizeof(int));
    int *rev_new = calloc(N,sizeof(int));
    int *fwd_new = calloc(N,sizeof(int));
    int *fwd_old = calloc(N,sizeof(int));

    // this function samples, reverses and does the union all at once
    for (int i=0; i<N; i++) {
        heap_t heap = B[i];
        int u = i;
// printf("i: %d, sum: %d\n", i, (B[i].fwd_new + B[i].fwd_old));
        // assert(B[i].fwd_new + B[i].fwd_old == 20);
        sum += B[i].fwd_new + B[i].fwd_old + B[i].rev_new + B[i].rev_old;


        for (int j=0; j<heap.size; j++) {
            bool isnew = heap.isnews[j];
            uint32_t v = heap.ids[j];
            if (isnew) {
                fwd_new[i]++;
                rev_new[v]++;
            } else {
                fwd_old[i]++;
                rev_old[v]++;
            }
        }
    }
    for (int i=0; i<N; i++) {
        heap_t heap = B[i];
        int u = i;
        assert(rev_old[i] == heap.rev_old);
        assert(rev_new[i] == heap.rev_new);
        assert(fwd_new[i] == heap.fwd_new);
        assert(fwd_old[i] == heap.fwd_old);
    }
    free(rev_old);
    free(rev_new);
    free(fwd_old);
    free(fwd_new);
    return sum;
}
