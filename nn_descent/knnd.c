#include "knnd.h"
#include "vec.h"

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

int make_test_data(dataset_t* data, const int n, const int d)
{
    data->values = malloc((sizeof(float*) * n) + (n * d * sizeof(float)));
    if (!(data->values)) return 1;

    for (int i = 0; i < n; i++) {
        data->values[i] = (float*)(data->values + n) + i * d;
        for (int j = 0; j < d; j++) {
            data->values[i][j] = (float)rand() / 100000000.f;
        }
    }

    data->size = n;
    data->dim = d;
    return 0;
}

vec_t* heap_list_create(const int size, const int k)
{
    // create list of 'size' many max heaps

    vec_t* hl = malloc(sizeof(vec_t) * size);
    hl->size = size;
    if (!hl) return NULL;

    for (int i = 0; i < size; i++) {
        if (!vec_create(&hl[i], k)) return NULL;
    }

    return hl;
}

void heap_list_free(vec_t* hl, const int size)
{
    for (int i = 0; i < size; i++) {
        vec_free(&hl[i]);
    }

    free(hl);
}

void max_heapify(vec_t* h, const int i)
{
    // enforce max-heap property after having removed maximum
    // the node at index i has replaced the old maximum as root
    // trickle down element i recursively

    // assumes that the heap always contains '_capacity' many elements

    const int l = (i * 2) + 1, r = (i * 2) + 2;
    int max;
    max = (l < h->_capacity && h->arr[l].val > h->arr[i].val) ? l : i;
    max = (r < h->_capacity && h->arr[r].val > h->arr[max].val) ? r : max;
    if (max != i) { //one of children is larger -> swap
        node_t t = h->arr[i];
        h->arr[i] = h->arr[max];
        h->arr[max] = t;
        max_heapify(h, max);
    }
}

int nn_update(vec_t* h, node_t* node)
{
    // insert node into maxheap h if it is more similar than the current root

    if (node->val >= h->arr[0].val || heap_find_by_index(h, node->id) != -1) return 0;
    h->arr[0] = *node;
    max_heapify(h, 0);
    return 1;
}

vec_t* nn_descent(dataset_t data, float(*metric)(float*, float*, int), const int k, const float rho, const float delta)
{
    // Implementation of Algorithm 2: NNDescentFull from the publication

    // metric: function returning disimilarity between two vectors of given dimension
    // rho: sample rate in (0,1] (2.5 Sampling)
    // delta: precision parameter, terminate if less than delta*K*N updates in an iteration

    // Scalar replacement with constant int
    const int tmp_data_size = data.size;
    const int tmp_data_dim = data.dim;

    if (k >= tmp_data_size) {
        printf("error: neighborhood size must be less than dataset size\n");
        return NULL;
    }

    // create lists with the maxheaps for each datapoint
    vec_t* B     = heap_list_create(tmp_data_size, k);
    vec_t* old   = heap_list_create(tmp_data_size, k);
    vec_t* new   = heap_list_create(tmp_data_size, k);

    if (!B || !old || !new) {
        printf("error: failed to allocate one or more heap lists\n");
        if (B) heap_list_free(B, tmp_data_size);
        if (old) heap_list_free(old, tmp_data_size);
        if (new) heap_list_free(new, tmp_data_size);
        return NULL;
    }

    // initialize heap list B
    for (int i = 0; i < tmp_data_size; i++) {
        // sample 
        for (int j = 0; j < k;) {
            node_t t = {(int)rand() % tmp_data_size, FLT_MAX, true};
            t.new = true;
            if (heap_insert(&B[i], &t) == 1) j++;
        }
    }

    int c; // number of updates in this iteration
    const int stop_iter = delta * tmp_data_size * k; // terminate if less than this many changes made
    const int max_candidates = 50;
    do {
        heap_list_free(old, tmp_data_size);
        heap_list_free(new, tmp_data_size);
        old = heap_list_create(tmp_data_size, max_candidates);
        new = heap_list_create(tmp_data_size, max_candidates);

        sample_reverse_union(new, old, B, max_candidates, tmp_data_size);

        c = 0;
        for (int v = 0; v < tmp_data_size; v++) {
            for (int i = 0; i < new[v].size; i++) {
                for (int k=0; k<2; k++) {
                    vec_t* heap_list = (k==0) ? new : old;

                    // Scalar replacement
                    int tmp_v_size = heap_list[v].size;

                    for (int j = 0; j < tmp_v_size; j++) {
                        if (i == j) continue;

                        node_t u1, u2;
                        u1.id = new[v].arr[i].id;
                        u2.id = heap_list[v].arr[j].id;

                        if (u1.id <= u2.id) continue; //|| heap_find_by_index(&old[v], u2.id) == -1) continue;
                        // smh never get here...

                        const float l = metric(data.values[u1.id], data.values[u2.id], tmp_data_dim);

                        u1.val = u2.val = l;
                        u1.new = u2.new = true;
                        c += nn_update(&B[u1.id], &u2);
                        c += nn_update(&B[u2.id], &u1);
                    }
                }    
            }
        }
       // printf("iteration complete: %d / %d\n", c, stop_iter);
    } while (c >= stop_iter);
    printf("NNDescent finished \n");
    //printf("done, cleaning up...\n");

    heap_list_free(old, tmp_data_size);
    heap_list_free(new, tmp_data_size);

    return B;
}

// inserts given node while keeping the number of elements in heap <= max_neighbors
int heap_insert_bounded(vec_t* h, node_t* node, const int max_neighbors)
{
    if (h->size<max_neighbors) {
        return heap_insert(h, node);
    } else {
        if (heap_find_by_index(h, node->id) >= 0)
            return 0;
        float curr_max = h->arr[0].val;
        if (node->val > curr_max)
            h->arr[0] = *node;
        max_heapify(h, 0); 
        return 1;
    }

}

int sample_reverse_union(vec_t* new, vec_t* old, vec_t* B, const int max_candidates, const int N) {
    // this function samples, reverses and does the union all at once
    for (int i=0; i<N; i++) {
        vec_t heap = B[i];
        int u = i;
        node_t *arr = heap.arr;

        for (int j=0; j<heap.size; j++) {
            // here we insert connections using some random weight in [0,1]
            // this corresponds to using the max_candidates-many neighbors with
            // the lowest (random) weight, which should be equivalent to sampling
            // max_candidates-many entries u.a.r. from the neighors of each node
            
            node_t n = arr[j];
            int v = n.id;
            float rand_weight = rand()/(float)RAND_MAX;

            // depending on flag select new or old heaplist
            vec_t* heap_list = (n.new) ? new : old;
            
            // existing connection
            (heap_insert_bounded(&heap_list[u], & (node_t) {v, rand_weight, n.new}, max_candidates));

            // reverse connection (add to B[v] if rand_weight amongst the K smallest...)
            (heap_insert_bounded(&heap_list[v], & (node_t) {u, rand_weight, n.new}, max_candidates));
        }
    }


    // set sampled to false 
    for (int i=0; i<N; i++) {
        vec_t heap = new[i];
        int u = i;
        node_t *arr = heap.arr;
        for (int j=0; j<heap.size; j++) {
            node_t n = arr[j];
            int v = n.id;
            int index = heap_find_by_index(&B[u], v);
            if (index >= 0)
                B[u].arr[index].new = false;

            index = heap_find_by_index(&B[v], u);
            if (index >= 0)
                B[v].arr[index].new = false;
        }
    }
    return 0;
}

