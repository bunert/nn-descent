#include "knnd.h"
#include "vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <sys/time.h>
#include <time.h>
#include "tsc_x86.h"

#define NUM_RUNS 10
#define CYCLES_REQUIRED 1e8
//TODO adapt frequency!
#define FREQUENCY 2.7e9

// #define CALIBRATE

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

vec_t* rdtsc(double *c, dataset_t data, float(*metric)(float*, float*, int), int k, float rho, float delta) {
    int i, num_runs;
    myInt64 cycles;
    myInt64 start;
    num_runs = NUM_RUNS;
    vec_t* B;

    /*
     * The CPUID instruction serializes the pipeline.
     * Using it, we can create execution barriers around the code we want to time.
     * The calibrate section is used to make the computation large enough so as to
     * avoid measurements bias due to the timing overhead.
     */
#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            nn_descent(data, metric, k, rho, delta);
        }
        cycles = stop_tsc(start);

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }
#endif

    // start = start_tsc();
    // for (i = 0; i < num_runs; ++i) {
    //     B = nn_descent(data, metric, k, rho, delta);
    // }
    // *c = (double) stop_tsc(start)/num_runs;

    start = start_tsc();

    B = nn_descent(data, metric, k, rho, delta);

    *c = (double) stop_tsc(start);

    return B;
}

vec_t* c_clock(double* c, dataset_t data, float(*metric)(float*, float*, int), int k, float rho, float delta) {
    int i, num_runs;
    double cycles;
    clock_t start, end;
    vec_t* B;

    num_runs = NUM_RUNS;

#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        start = clock();
        for (i = 0; i < num_runs; ++i) {
            nn_descent(data, metric, k, rho, delta);
        }
        end = clock();

        cycles = (double)(end-start);

        // Same as in c_clock: CYCLES_REQUIRED should be expressed accordingly to the order of magnitude of CLOCKS_PER_SEC
        if(cycles >= CYCLES_REQUIRED/(FREQUENCY/CLOCKS_PER_SEC)) break;

        num_runs *= 2;
    }
#endif


    start = clock();
    for(i=0; i<num_runs; ++i) {
        B = nn_descent(data, metric, k, rho, delta);
    }
    end = clock();
    *c = (double)(end-start)/num_runs;
    return B;
}


vec_t* timeofday(double* c, dataset_t data, float(*metric)(float*, float*, int), int k, float rho, float delta) {
    int i, num_runs;
    double cycles;
    struct timeval start, end;
    vec_t* B;

    num_runs = NUM_RUNS;

#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        gettimeofday(&start, NULL);
        for (i = 0; i < num_runs; ++i) {
            nn_descent(data, metric, k, 1.0, 0.001);
        }
        gettimeofday(&end, NULL);

        cycles = (double)((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6)*FREQUENCY;

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }
#endif


    gettimeofday(&start, NULL);
    for(i=0; i < num_runs; ++i) {
        B = nn_descent(data, metric, k, 1.0, 0.001);
    }
    gettimeofday(&end, NULL);
    *c = (double)((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6)/ num_runs;
    return B;
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

    double *c = malloc(sizeof(double));

    vec_t* B;

    // 3 timing methods, use the first one as long as no overflow appears

    // RDTSC instruction:\n %lf cycles measured => %lf seconds,
    // assuming frequency is %lf MHz. (change in source file if different)
    B = rdtsc(c, data, &l2, K, 1.0, 0.001);
    printf("Timing-method RDTSC \n%0.1lf cycles \n%lf seconds \n%0.1lf MHz\n\n", *c, *c/(FREQUENCY), (FREQUENCY)/1e6);

    // C clock() function:\n %lf cycles measured. On some systems,
    // this number seems to be actually computed from a timer in
    // seconds then transformed into clock ticks using the variable CLOCKS_PER_SEC.
    // Unfortunately, it appears that CLOCKS_PER_SEC is sometimes set improperly.
    // (According to this variable, your computer should be running at %lf MHz).
    // In any case, dividing by this value should give a correct timing: %lf seconds.
    // B = c_clock(c, data, &l2, K, 1.0, 0.001);
    // printf("C clock() function:\n %lf cycles \n %lf seconds \n %lf MHz \n\n",*c, *c/CLOCKS_PER_SEC, (double) (CLOCKS_PER_SEC)/1e6);


    // B = timeofday(c, data, &l2, K, 1.0, 0.001);
    // printf("C gettimeofday() function:\n %lf seconds \n\n",*c);

    // B = nn_descent(data, &l2, K, 1.0, 0.001);

    for (int i = 0; i < data.size; i++) {
        heap_check(&B[i], 0);
    }
    write_data(outputfname, N, B);

    heap_list_free(B, data.size);
    free(data.values);


}
