#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "tsc_x86.h"

#define NUM_RUNS 1
#define CYCLES_REQUIRED 1e8
//TODO adapt frequency!
#define FREQUENCY 2.7e9


// test
vec_t* nn_descent(dataset_t data, float(*metric)(float*, float*, int), int k, float rho, float delta) {
    return heap_list_create(data.size, k);
}

/*
 * Timing function based on the TimeStep Counter of the CPU.
 */

double rdtsc(dataset_t data, float(*metric)(float*, float*, int), int k, float rho, float delta) {
    int i, num_runs;
    myInt64 cycles;
    myInt64 start;
    num_runs = NUM_RUNS;

    /*
     * The CPUID instruction serializes the pipeline.
     * Using it, we can create execution barriers around the code we want to time.
     * The calibrate section is used to make the computation large enough so as to
     * avoid measurements bias due to the timing overhead.
     */
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            nn_descent(data, metric, 20, 1.0, 0.001);
        }
        cycles = stop_tsc(start);

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }


    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
        nn_descent(data, metric, 20, 1.0, 0.001);
    }

    cycles = stop_tsc(start)/num_runs;
    return (double) cycles;
}

double c_clock(dataset_t data, float(*metric)(float*, float*, int), int k, float rho, float delta) {
    int i, num_runs;
    double cycles;
    clock_t start, end;

    num_runs = NUM_RUNS;

    while(num_runs < (1 << 14)) {
        start = clock();
        for (i = 0; i < num_runs; ++i) {
            nn_descent(data, metric, 20, 1.0, 0.001);
        }
        end = clock();

        cycles = (double)(end-start);

        // Same as in c_clock: CYCLES_REQUIRED should be expressed accordingly to the order of magnitude of CLOCKS_PER_SEC
        if(cycles >= CYCLES_REQUIRED/(FREQUENCY/CLOCKS_PER_SEC)) break;

        num_runs *= 2;
    }


    start = clock();
    for(i=0; i<num_runs; ++i) {
        nn_descent(data, metric, 20, 1.0, 0.001);
    }
    end = clock();

    return (double)(end-start)/num_runs;
}


double timeofday(dataset_t data, float(*metric)(float*, float*, int), int k, float rho, float delta) {
    int i, num_runs;
    double cycles;
    struct timeval start, end;

    num_runs = NUM_RUNS;

    while(num_runs < (1 << 14)) {
        gettimeofday(&start, NULL);
        for (i = 0; i < num_runs; ++i) {
            nn_descent(data, metric, 20, 1.0, 0.001);
        }
        gettimeofday(&end, NULL);

        cycles = (double)((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6)*FREQUENCY;

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }


    gettimeofday(&start, NULL);
    for(i=0; i < num_runs; ++i) {
        nn_descent(data, metric, 20, 1.0, 0.001);
    }
    gettimeofday(&end, NULL);

    return (double)((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6)/ num_runs;
}


int main(int argc, char **argv) {
    if (argc!=2) {printf("usage: FW <n>\n"); return -1;}
    int n = atoi(argv[1]);
    printf("n=%d \n",n);

    vec_t*

    double r = rdtsc(data, &l2, 20, 1.0, 0.001);
    printf("RDTSC instruction:\n %lf cycles measured => %lf seconds, assuming frequency is %lf MHz. (change in source file if different)\n\n", r, r/(FREQUENCY), (FREQUENCY)/1e6);


    double c = c_clock(data, &l2, 20, 1.0, 0.001);
    printf("C clock() function:\n %lf cycles measured. On some systems, this number seems to be actually computed from a timer in seconds then transformed into clock ticks using the variable CLOCKS_PER_SEC. Unfortunately, it appears that CLOCKS_PER_SEC is sometimes set improperly. (According to this variable, your computer should be running at %lf MHz). In any case, dividing by this value should give a correct timing: %lf seconds. \n\n",c, (double) CLOCKS_PER_SEC/1e6, c/CLOCKS_PER_SEC);


    double t = timeofday(data, &l2, 20, 1.0, 0.001);
    printf("C gettimeofday() function:\n %lf seconds measured\n\n",t);

    return 0;
}
