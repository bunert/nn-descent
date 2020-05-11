#include "vec.h"
#include "knnd.h"

int nn_brute_force(float(*metric)(float*, float*, int), dataset_t data, update_t* updates, int update_index, vec_t *vec_a, vec_t *vec_b);
