#include "knnd.h"
#include "vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

float l2(float* v1, float* v2, int d)
{
    float acc = 0.0f;
    for (int i = 0; i < d; i++) {
        acc += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }

    return sqrt(acc);
}

void heap_check(vec_t* h, int i)
{
    int l = i*2 + 1, r = i*2 + 2;
    if (l < h->_capacity) {
        assert(h->arr[i].val >= h->arr[l].val);
        heap_check(h, l);
    }

    if (r < h->_capacity) {
        assert(h->arr[i].val >= h->arr[r].val);
        heap_check(h, r);
    }
}

void read_data(char* filename, int N, int D, dataset_t* data) {
    data->values = malloc((sizeof(float*) * N) + (N * D * sizeof(float)));
    data->size = N;
    data->dim = D;
    FILE *fp;
    if ((fp = fopen(filename, "r+")) != NULL) {
        for (int i = 0; i < N; i++) {
            data->values[i] = (float*)(data->values + N) + i * D;
            for (int j = 0; j < D; j++) {

                // returns 0 if we couldnt read float
                if (fscanf(fp, "%f", data->values[i] + j) == EOF)
                    exit(1);
            }
        }
        fclose(fp);
    } else {
        exit(1);
    }
}

void write_data(char* filename, int N, vec_t* result) {
    FILE *fp;
    if ((fp = fopen(filename, "w")) != NULL) {
        for (int i=0; i<N; i++) {
            for (int j=0; j<result[i].size; j++) {
                fprintf(fp, "%d", result[i].arr[j].id);
                if (j!=result[i].size-1)
                    fputs(" ", fp);
            }
            fputs("\n", fp);
        }
        fclose(fp);
    } else {
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 6) {
        printf("Usage: <inputfilename> <outputfilename> N D K");
        return 0;
    }
    char* fname = argv[1];
    char* outputfname = argv[2];
    int N = atoi(argv[3]);
    int D = atoi(argv[4]);
    int K = atoi(argv[5]);

    dataset_t data;
    read_data(fname, N, D, &data);

    vec_t* B = nn_descent(data, &l2, K, 1.0, 0.001);

    for (int i = 0; i < data.size; i++) {
        heap_check(&B[i], 0);
    }
    write_data(outputfname, N, B);

    heap_list_free(B, data.size);
    free(data.values);
}
