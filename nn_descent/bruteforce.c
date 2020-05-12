#include "knnd.h"
#include "vec.h"
#include "bruteforce.h"

int nn_brute_force(float(*metric)(float*, float*, int), dataset_t data, update_t* updates, int update_index, vec_t *vec_a, vec_t *vec_b){
  for (int i = 0; i < vec_a->size; i++) {
      for (int j = 0; j < vec_b->size; j++) {
          if (i == j) continue;

          uint32_t u1_id, u2_id;
          u1_id = vec_a->ids[i];
          u2_id = vec_b->ids[j];

          if (u1_id <= u2_id) continue;

          float l = metric(data.values[u1_id], data.values[u2_id], data.dim);
          updates[update_index++] = (update_t) {u1_id, u2_id, l};


      }
  }
  return update_index;
}
