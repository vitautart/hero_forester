#include "common.h"

#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

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

int main()
{
    test_bitset_allocate();
    test_bitset_get_set();
    test_dynarr_full();
    test_dynarr_access();
    test_dynarr_remove();
    test_dynarr_swap();
    test_minheap_push_pop();
    return 0;
}
