#include <stdio.h>
#include "heap_list.h"
#include <assert.h>
#include <stdlib.h>

void heap_test() {
    printf("Starting heap test \n");
    heap_t* hp = heap_init(1, 1);

    heap_insert(hp, &(heap_el_t){-1.0});
    heap_insert(hp, &(heap_el_t){0.0});
    heap_insert(hp, &(heap_el_t){5.0});
    heap_insert(hp, &(heap_el_t){1.0});
    heap_insert(hp, &(heap_el_t){4.0});
    heap_insert(hp, &(heap_el_t){-1.0});
    heap_insert(hp, &(heap_el_t){4.0});
    heap_insert(hp, &(heap_el_t){0.0});
    heap_insert(hp, &(heap_el_t){1.0});
    heap_insert(hp, &(heap_el_t){5.0});


    assert(heap_extract(hp)->key == 5.0);

    heap_insert(hp, &(heap_el_t){1000.0});

    double expected_els[] = {1000.0, 5.0, 4.0, 4.0, 1.0, 1.0, 0.0, 0.0, -1.0, -1.0};
    
    for (int i=0; i<10; i++) {
        assert(heap_extract(hp)->key == expected_els[i]);
        printf("extracted: %f\n", expected_els[i]);
    }
    printf("Done heap test \n");
}

void heap_list_test() {
    heap_list_t* src = heap_list_init(10);
    heap_list_t* dst = heap_list_init(10);
    heap_list_t* dst2 = heap_list_init(10);

    heap_t* h = heap_init(1, 1);
    heap_insert(h, &(heap_el_t){0.0, 2});
    heap_insert(h, &(heap_el_t){0.0, 3});
    heap_insert(h, &(heap_el_t){0.0, 4});

    heap_list_insert(src, h);

    h = heap_init(3, 5);
    heap_insert(h, &(heap_el_t){0.0, 2, true});
    heap_insert(h, &(heap_el_t){0.0, 3});
    heap_insert(h, &(heap_el_t){0.0, 6});

    heap_list_insert(src, h);
    heap_list_reverse(src, dst);
    heap_list_reverse(dst, dst2);

    // reversing twice should be id
    heap_list_print(src);
    heap_list_print(dst);
    heap_list_print(dst2);
    assert(heap_list_eq(src, dst2));

}

void read_data(double* points) {
    FILE *fp;
    double* read_ptr = points;
    if ((fp = fopen("data", "r+")) != NULL) {
        int ret = fscanf(fp, "%lf %lf %lf %lf", read_ptr, read_ptr+1, read_ptr+2, read_ptr+3); 
        while (ret == 4) { 
            read_ptr+=4;
            ret = fscanf(fp, "%lf %lf %lf %lf", read_ptr, read_ptr+1, read_ptr+2, read_ptr+3); 
        }
    }
}

void sample(int arr[], int size, int max) {
    for (int i=0; i<size; i++)
    {
        int j=0;

        do {
            arr[i] = rand() % max;
            // printf("%d\n", arr[i]);
            for (j=0; j<i; j++)
                if (arr[j] == arr[i])
                    break;
            
        } while (j!=i);
    }
}

double distance(double* points, int n1, int n2) {
    return (points[4*n1]-points[4*n2])*(points[4*n1]-points[4*n2])
        + (points[4*n1+1]-points[4*n2+1])*(points[4*n1+1]-points[4*n2+1])
        + (points[4*n1+2]-points[4*n2+2])*(points[4*n1+2]-points[4*n2+2])
        + (points[4*n1+3]-points[4*n2+3])*(points[4*n1+3]-points[4*n2+3]);
}

int update_nn(heap_t* h, int node_id, double key) {

    assert(!heap_empty(h));
    if (heap_empty(h))
        return 0;

    assert(h->id != node_id);
    if (heap_peek(h)->key > key) {
        
        heap_el_t* el = heap_extract(h);
        el->key = key;
        el->node_id = node_id;
        el->newf = true;
        heap_insert(h, el);
        return 1;
    }
    return 0;
}

int main() {
    // TODO: don't do the modifications on the B we're working on
    // TODO: check whether split into new/old is necessary
    heap_test();
    heap_list_test();
    
    double* points = malloc(4*1000*4*sizeof(double));
    read_data(points); 

    int K = 10;
    int T = 10;
    int N = 4*1000;
    int* indices = malloc(K*sizeof(int));

    heap_list_t* B = heap_list_init(N);
    heap_list_t* R = heap_list_init(N);

    // initialize every node with random K nearest neighbors
    for (int i=0; i<N; i++) {
        sample(indices, K, N);
        heap_t* adj_heap = heap_init(K, i);
        for (int j=0; j<K; j++) {
            heap_el_t* hel = malloc(sizeof(heap_el_t));
            hel->node_id = indices[j];
            hel->key = distance(points, i, indices[j]);
            hel->newf = true;
            
            heap_insert(adj_heap, hel);
        }

        heap_list_insert(B, adj_heap); 
    }
    printf("Done initializing...\n");

    for (int t=0; t<T; t++) {
        int c=0;
        heap_list_reverse(B, R);
        for (int i=0; i<N; i++) {
            heap_t* B_i = heap_list_get(B, i);
            heap_t* R_i = heap_list_get(R, i);

            int union_length = B_i->max_indx;
            // we might not have any edges pointing to node i
            // but we will always have K pointing outwards from node i
            if (R_i)
                union_length += R_i->max_indx;

            for (int j=0; j< union_length; j++) {
                heap_el_t* el_j = (j < B_i->max_indx) ?
                    B_i->els[j] : R_i->els[j-B_i->max_indx];

                for (int k=j+1; k< union_length; k++) {
                    heap_el_t* el_k = (k < B_i->max_indx) ?
                        B_i->els[k] : R_i->els[k-B_i->max_indx];

                    if ((el_k->newf||el_j->newf)
                            && el_k->node_id != el_j->node_id) {
                        double l = distance(points, el_j->node_id, el_k->node_id);
                        c += update_nn(heap_list_get(B,el_j->node_id), el_k->node_id, l);
                        c += update_nn(heap_list_get(B,el_k->node_id), el_j->node_id, l);
                    }
                }
            }

        }
        printf("C: %d\n", c);

    }
        double correct = 0;

        for (int i=0; i<N; i++) {
            assert(heap_peek(heap_list_get(B,i))->node_id!=i);
            if (heap_peek(heap_list_get(B, i))->node_id/1000 == i/1000)
                correct += 1;
        }
        printf("Closest neighbor from same distribution: %f\n", correct/N);

}
