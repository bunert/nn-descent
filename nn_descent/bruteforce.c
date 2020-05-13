#include "knnd.h"
#include "vec.h"
#include "bruteforce.h"
#include <stdio.h>
#include <assert.h>
#include <immintrin.h>
#include <math.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#ifdef INSTR

 #pragma GCC push_options
 #pragma GCC optimize ("O0")
 
 void sim_eval() {
 }
 #pragma GCC pop_options

 #define DIST_EVAL() sim_eval()
 #else
 #define DIST_EVAL() true
 #endif
 
inline float single_l2(float* v1, float* v2, int d)
{
    float acc = 0.0f;
    float acc2 = 0.0f;
    float acc3 = 0.0f;
    float acc4 = 0.0f;
    int offset=d/4;
    for (int i = 0; i < offset; i++) {
        acc += (v1[i] - v2[i]) * (v1[i] - v2[i]);
        acc2 += (v1[offset+i] - v2[offset+i]) * (v1[offset+i] - v2[offset+i]);
        acc3 += (v1[2*offset+i] - v2[2*offset+i]) * (v1[2*offset+i] - v2[2*offset+i]);
        acc4 += (v1[3*offset+i] - v2[3*offset+i]) * (v1[3*offset+i] - v2[3*offset+i]);
    }
    for (int i = 4*offset; i < d; i++) {
        acc += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }

    return acc+acc2+acc3+acc4;
}
 


void nn_brute_force(float(*metric)(float*, float*, int), dataset_t data, update_t* updates, vec_t *vec_a, vec_t *vec_b){
    uint32_t u1_id[8];
    uint32_t u2_id[8];
    int agg_cnt=0;

    for (int i = 0; i < vec_a->size; i++) {


        // if they match, can only do upper triangle due to symmetry of euclidean distance
        int start = (vec_a == vec_b) ? i+1 : 0;
        for (int j = start; j < vec_b->size; j++) {
            u1_id[agg_cnt] = vec_a->ids[i];
            u2_id[agg_cnt] = vec_b->ids[j];
            DIST_EVAL();
            agg_cnt++;
            if (agg_cnt < 8) continue;
            agg_cnt = 0;

            __m256 acc[8];
            for (int l=0; l<8; l++) {
                acc[l] = _mm256_setzero_ps();
            }

            for (int k=0; k < data.dim; k+=8) {

                __m256 x0[8], x1[8], x2[8];
                for (int l=0; l<8; l++) {
                    x0[l] = _mm256_loadu_ps(&(data.values[u1_id[l]][k]));
                    x1[l] = _mm256_loadu_ps(&(data.values[u2_id[l]][k]));
                    x2[l] = _mm256_sub_ps(x0[l], x1[l]);
                    acc[l] = _mm256_fmadd_ps(x2[l], x2[l], acc[l]);
                }
            }
            __m256 y01 = _mm256_hadd_ps(acc[0],acc[1]);
            __m256 y23 = _mm256_hadd_ps(acc[2],acc[3]);
            __m256 y45 = _mm256_hadd_ps(acc[4],acc[5]);
            __m256 y67 = _mm256_hadd_ps(acc[6],acc[7]);

            __m256 z0123 = _mm256_hadd_ps(y01,y23);
            __m256 z4567 = _mm256_hadd_ps(y45,y67);

            __m256 z45670123 = _mm256_blend_ps(z0123, z4567,  0b00001111);
            __m256 z45670123_flipped = _mm256_permute2f128_ps(z45670123, z45670123, 1);
            __m256 z01234567 = _mm256_blend_ps(z0123, z4567, 0b11110000);

            __m256 z = _mm256_add_ps(z01234567, z45670123_flipped);

            _mm256_storeu_si256((__m256i *)&(updates->u[updates->size]), _mm256_set_epi32(u1_id[7],u1_id[6],u1_id[5],u1_id[4],u1_id[3],u1_id[2],u1_id[1],u1_id[0]));
            _mm256_storeu_si256((__m256i *)&(updates->v[updates->size]), _mm256_set_epi32(u2_id[7],u2_id[6],u2_id[5],u2_id[4],u2_id[3],u2_id[2],u2_id[1],u2_id[0]));
            _mm256_storeu_ps(&(updates->dist[updates->size]), z);
            updates->size += 8;


        }

    }

    for (int j = 0; j < agg_cnt; j++) {
        float l = single_l2(data.values[u1_id[j]], data.values[u2_id[j]], data.dim);
            int ind = updates->size;
            updates->u[ind] = u1_id[j];
            updates->v[ind] = u2_id[j];
            updates->dist[ind] = l;
            updates->size++;
    }
    /*
       float l = metric(data.values[u1_id], data.values[u2_id], data.dim);
       updates[update_index++] = (update_t) {u1_id, u2_id, l};*/

}


            // x = _mm256_permute2f128_ps( x , x , 1)
/*
            {
                int l=5;

                float sum = ((float*)&acc[l])[0] +
                    ((float*)&acc[l])[1] +
                    ((float*)&acc[l])[2] +
                    ((float*)&acc[l])[3] +
                    ((float*)&acc[l])[4] +
                    ((float*)&acc[l])[5] +
                    ((float*)&acc[l])[6] +
                    ((float*)&acc[l])[7];
                printf("%f, %f\n", sum, z0123[0]+z0123[4]);
                printf("%f, %f\n", sum, z[5]);
                return;
                assert(sum==z[0]);
                assert(sum==((float*)&z0123)[0]+((float*)&z0123)[4]);

            /*
            {
                int l=0;

                float sum = ((float*)&acc[l])[0] +
                    ((float*)&acc[l])[1] +
                    ((float*)&acc[l])[2] +
                    ((float*)&acc[l])[3] +
                    ((float*)&acc[l])[4] +
                    ((float*)&acc[l])[5] +
                    ((float*)&acc[l])[6] +
                    ((float*)&acc[l])[7];
                assert(sum==((float*)&y01)[0]+((float*)&y01)[1]
                + ((float*)&y01)[4]+((float*)&y01)[5]);
            }*/
/*
            for (int l=0; l<8; l++) {
                float sum = ((float*)&acc[l])[0] +
                    ((float*)&acc[l])[1] +
                    ((float*)&acc[l])[2] +
                    ((float*)&acc[l])[3] +
                    ((float*)&acc[l])[4] +
                    ((float*)&acc[l])[5] +
                    ((float*)&acc[l])[6] +
                    ((float*)&acc[l])[7];
                updates[update_index++] = (update_t) {u1_id, u2_id[l], sum};
            }*/

/*            }*/
