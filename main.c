#include <stdio.h>
#include "heap.h"

int main() {
    heap_t* hp = heap_init(10);

    heap_insert(hp, &(heap_el_t){-1.0});
    heap_insert(hp, &(heap_el_t){0.0});
    heap_insert(hp, &(heap_el_t){1.0});
    heap_insert(hp, &(heap_el_t){5.0});
    heap_insert(hp, &(heap_el_t){4.0});
    heap_insert(hp, &(heap_el_t){-1.0});
    heap_insert(hp, &(heap_el_t){0.0});
    heap_insert(hp, &(heap_el_t){1.0});
    heap_insert(hp, &(heap_el_t){5.0});
    heap_insert(hp, &(heap_el_t){4.0});

    for (int i=0; i<10; i++) {
        printf("extracted: %f\n", heap_extract(hp)->key);
    }
}
