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
    printf("test_dynarr_access:\tSUCCESS\n");
}

int main()
{
    test_bitset_allocate();
    test_bitset_get_set();
    test_dynarr_full();
    test_dynarr_access();
    return 0;
}
