#include "common.h"

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

//#define TRY_TESTS

void test_bitset_allocate()
{
    bitset_t set = bitset_allocate(65);
    memset(set.cells, 0, set.bytecount);
    for (int i = 0; i < set.cellcount; i++) assert(set.cells[i] == 0);
    assert(set.cells != NULL);
    assert(set.cellcount == 3);
    assert(set.bitcount == 65);
    bitset_free(&set);

    set = bitset_allocate(138);
    memset(set.cells, 0, set.bytecount);
    for (int i = 0; i < set.cellcount; i++) assert(set.cells[i] == 0);
    assert(set.cells != NULL);
    assert(set.cellcount == 5);
    assert(set.bitcount == 138);
    bitset_free(&set);

    set = bitset_allocate(64);
    memset(set.cells, 0, set.bytecount);
    for (int i = 0; i < set.cellcount; i++) assert(set.cells[i] == 0);
    assert(set.cells != NULL);
    assert(set.cellcount == 2);
    assert(set.bitcount == 64);
    bitset_free(&set);

    printf("test_bitset_allocate:\tSUCCESS\n");
}

void test_bitset_get_set()
{
    int size = 168;
    bitset_t set = bitset_allocate(size);

    time_t t;
    srand((unsigned) time(&t)); rand(); rand();
    for (int i = 0; i < set.cellcount; i++) set.cells[i] = rand();

    bitset_t set_backup = bitset_allocate(size);
    memcpy(set_backup.cells, set.cells, set.bytecount);

    int idx = 19;
    bitset_set(&set, idx, 2);
    assert(bitset_get(&set, idx) != 0);
    bitset_set(&set, idx, 0);
    assert(bitset_get(&set, idx) == 0);
    for (int i = 0; i < set_backup.bitcount; i++)
        if (i != idx)
            assert(bitset_get(&set, i) == bitset_get(&set_backup, i));

    idx = 0;
    bitset_set(&set, idx, 1 << 28);
    assert(bitset_get(&set, idx) != 0);
    bitset_set(&set, idx, 0);
    assert(bitset_get(&set, idx) == 0);

    idx = size - 1;
    bitset_set(&set, idx, 1);
    assert(bitset_get(&set, idx) != 0);
    bitset_set(&set, idx, 0);
    assert(bitset_get(&set, idx) == 0);

    bitset_free(&set_backup);
    bitset_free(&set);

    printf("test_bitset_get_set:\tSUCCESS\n");
}

void test_dynarr_full()
{
    dynarr_t arr = dynarr_allocate(sizeof(int), 0, 0);
    for (int i = 0; i < 5; i++) 
        dynarr_add(&arr, &i);

    for (int i = 0; i < arr.size; i++)
        assert(GET_AS(int, dynarr_get(&arr, i)) == i);
    
    for (int i = arr.size - 1; i >= 0; i--)
    {
        int last = GET_AS(int, dynarr_get(&arr, arr.size - 1)); 
        dynarr_remove_swap(&arr, i);
        assert(GET_AS(int, dynarr_get(&arr, i)) == last);
    }
    assert(arr.size == 0);

    dynarr_free(&arr);

    arr = dynarr_allocate(sizeof(int), 5, 4);

    dynarr_set(&arr, 2, &(int){10});
    assert(GET_AS(int, dynarr_get(&arr, 2)) == 10);

    dynarr_free(&arr);

    printf("test_dynarr_full:\tSUCCESS\n");
}

void test_dynarr_access()
{
    dynarr_t arr[2] = { dynarr_allocate(sizeof(int), 10, 10), dynarr_allocate(sizeof(int), 0, 0) };
    dynarr_t *ref = &arr[1];
    for (int i = 0; i < 5; i++) 
        dynarr_add(ref, &i);

    assert(arr[1].size == 5);

    dynarr_free(&arr[0]);
    dynarr_free(&arr[1]);
    printf("test_dynarr_access:\tSUCCESS\n");
}

void test_dynarr_remove()
{
    dynarr_t arr = dynarr_allocate(sizeof(int), 0, 0);
    for (int i = 0; i < 10; i++) 
        dynarr_add(&arr, &i);

    dynarr_remove(&arr, 0);

    for (int i = 0; i < arr.size; i++)
        assert(GET_AS(int, dynarr_get(&arr, i)) == i+1);

    dynarr_remove(&arr, arr.size - 1);

    for (int i = 0; i < arr.size; i++)
        assert(GET_AS(int, dynarr_get(&arr, i)) == i+1);

    int idx_to_remove = arr.size / 2;

    dynarr_remove(&arr, idx_to_remove);

    for (int i = 0; i < idx_to_remove; i++)
        assert(GET_AS(int, dynarr_get(&arr, i)) == i+1);

    for (int i = idx_to_remove; i < arr.size; i++)
        assert(GET_AS(int, dynarr_get(&arr, i)) == i+2);

    dynarr_free(&arr);
    printf("test_dynarr_remove:\tSUCCESS\n");
}

void test_dynarr_swap()
{
    dynarr_t arr = dynarr_allocate(sizeof(int), 0, 0);
    for (int i = 0; i < 10; i++) 
        dynarr_add(&arr, &i);
    {
        int first_idx = 0;
        int second_idx = arr.size - 1;
        int first_value = GET_AS(int, dynarr_get(&arr, first_idx));
        int second_value = GET_AS(int, dynarr_get(&arr, second_idx));
        dynarr_swap(&arr, first_idx, second_idx);
        int first_value_swaped = GET_AS(int, dynarr_get(&arr, first_idx));
        int second_value_swaped = GET_AS(int, dynarr_get(&arr, second_idx));
        assert(first_value_swaped == second_value);
        assert(second_value_swaped == first_value);
    }
    {
        int first_idx = 2;
        int second_idx = 5;
        int first_value = GET_AS(int, dynarr_get(&arr, first_idx));
        int second_value = GET_AS(int, dynarr_get(&arr, second_idx));
        dynarr_swap(&arr, first_idx, second_idx);
        int first_value_swaped = GET_AS(int, dynarr_get(&arr, first_idx));
        int second_value_swaped = GET_AS(int, dynarr_get(&arr, second_idx));
        assert(first_value_swaped == second_value);
        assert(second_value_swaped == first_value);
    }
    {
        int first_idx = 0;
        int second_idx = 0;
        int first_value = GET_AS(int, dynarr_get(&arr, first_idx));
        int second_value = GET_AS(int, dynarr_get(&arr, second_idx));
        dynarr_swap(&arr, first_idx, second_idx);
        int first_value_swaped = GET_AS(int, dynarr_get(&arr, first_idx));
        int second_value_swaped = GET_AS(int, dynarr_get(&arr, second_idx));
        assert(first_value_swaped == second_value);
        assert(second_value_swaped == first_value);
    }
    dynarr_free(&arr);
    printf("test_dynarr_swap:\tSUCCESS\n");
}

void test_minheap_push_pop()
{
    qnode_t nodes[10];

    for(int i = 0; i < 10; i++) 
    {
        nodes[i].key = i;
        nodes[i].value = i;
    };

    minheap_t heap = minheap_allocate(10);
    for (int i = 9; i >= 0; i--)
        minheap_push(&heap, nodes[i]);

    for (int i = 0; i < 10; i++)
    {
        qnode_t node;
        minheap_pop(&heap, &node);
        assert(node.key == i);
    }

    qnode_t node;
    assert(minheap_pop(&heap, &node) == 0);

    minheap_push(&heap, nodes[1]);
    minheap_push(&heap, nodes[5]);
    minheap_push(&heap, nodes[2]);

    assert(minheap_pop(&heap, &node) == 1);
    assert(node.key == 1);
    assert(minheap_pop(&heap, &node) == 1);
    assert(node.key == 2);
    assert(minheap_pop(&heap, &node) == 1);
    assert(node.key == 5);

    minheap_free(&heap);
    printf("test_minheap_push_pop:\tSUCCESS\n");
}

void test_hashmap_add_get()
{
    hashmap_t map = hashmap_allocate(30, 50);

    for (int i = 0; i < 50; i++)
    {
        qnode_t node = {i, i};
        hashmap_add_or_replace(&map, node);
    }

    for (int i = 50; i < 100; i++)
    {
        qnode_t node = {i, i};
        assert(hashmap_try_add(&map, node));
    }

    assert(!hashmap_try_add(&map, (qnode_t){5, 5}));

    int value;
    for (int i = 0; i < 100; i++)
    {
        assert(hashmap_get(&map, i, &value));
        assert(value == i);
    }

    assert(!hashmap_get(&map, 100, &value));


    hashmap_add_or_replace(&map, (qnode_t){6, 10});
    hashmap_get(&map, 6, &value);
    assert(value == 10);

    hashmap_free(&map);

    printf("test_hashmap_add_get:\tSUCCESS\n");
}

void test_hashmap_remove()
{
    hashmap_t map = hashmap_allocate(30, 50);

    for (int i = 0; i < 100; i++)
    {
        qnode_t node = {i, i};
        hashmap_add_or_replace(&map, node);
    }

    int value;

    for (int i = 0; i < 100; i++)
    {
        assert(hashmap_remove(&map, i));
        assert(!hashmap_get(&map, i, &value));
    }

    assert(!hashmap_remove(&map, 2));

    hashmap_free(&map);

    printf("test_hashmap_remove:\tSUCCESS\n");
}

void test_knuth_mult_hash_collisions()
{
    int size = 100;
    int collisions_per_bucket[size];
    memset(collisions_per_bucket, 0, sizeof(int) * size);

    for (int i = 0; i < 100000; i++)
    {
        int id = knuth_mult_hash(i) % size;
        collisions_per_bucket[id] = collisions_per_bucket[id] + 1;
    }

    for (int i = 0; i < size; i++)
    {
        printf("id: %i\t\tcollisions: %i\n", i, collisions_per_bucket[i]);
    }
}

int main()
{
    test_bitset_allocate();
    test_bitset_get_set();
    test_dynarr_full();
    test_dynarr_access();
    test_dynarr_remove();
    test_dynarr_swap();
    test_minheap_push_pop();
    test_hashmap_add_get();
    test_hashmap_remove();

#ifdef TRY_TESTS
    test_knuth_mult_hash_collisions();
#endif

    return 0;
}
