#include <stdio.h>
#include "heap_list.h"
#include <assert.h>

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
    heap_list_t* src = heap_list_init(1);
    heap_list_t* dst = heap_list_init(1);
    heap_list_t* dst2 = heap_list_init(1);

    heap_t* h = heap_init(1, 1);
    heap_insert(h, &(heap_el_t){0.0, 2});
    heap_insert(h, &(heap_el_t){0.0, 3});
    heap_insert(h, &(heap_el_t){0.0, 4});

    heap_list_insert(src, h);

    h = heap_init(3, 5);
    heap_insert(h, &(heap_el_t){0.0, 2});
    heap_insert(h, &(heap_el_t){0.0, 3});
    heap_insert(h, &(heap_el_t){0.0, 6});

    heap_list_insert(src, h);
    heap_list_reverse(src, dst);
    heap_list_reverse(dst, dst2);

    // reversing twice should be id
    assert(heap_list_eq(src, dst2));
    heap_list_print(src);
    heap_list_print(dst);
    heap_list_print(dst2);
}

int main() {
    heap_test();
    heap_list_test();
}

