#include <assert.h>
#define UNIT_TEST
#include "common.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

void test_bitset_allocate()
{
    bitset_t set = bitset_allocate(65);
    assert(set.data != NULL);
    assert(set.data_size == 3);
    assert(set.elem_size_in_bits == 32);
    assert(set.elem_count == 65);
    for (int i = 0; i < set.data_size; i++) assert(set.data[i] == 0);
    bitset_free(&set);

    set = bitset_allocate(138);
    assert(set.data != NULL);
    assert(set.data_size == 5);
    assert(set.elem_size_in_bits == 32);
    assert(set.elem_count == 138);
    bitset_free(&set);

    set = bitset_allocate(64);
    assert(set.data != NULL);
    assert(set.data_size == 2);
    assert(set.elem_size_in_bits == 32);
    assert(set.elem_count == 64);
    bitset_free(&set);

    printf("test_bitset_allocate: SUCCESS\n");
}

void test_bitset_get_set()
{
    int size = 168;
    bitset_t set = bitset_allocate(size);

    time_t t;
    srand((unsigned) time(&t));
    rand();
    rand();
    for (int i = 0; i < set.data_size; i++) set.data[i] = rand();

    bitset_t set_backup = bitset_allocate(size);
    memcpy(set_backup.data, set.data, (sizeof *set.data) * set.data_size);

    int idx = 19;
    bitset_set(&set, idx, 2);
    assert(bitset_get(&set, idx) != 0);
    bitset_set(&set, idx, 0);
    assert(bitset_get(&set, idx) == 0);
    for (int i = 0; i < set_backup.elem_count; i++)
    {
        if (i != idx)
        {
            assert(bitset_get(&set, i) == bitset_get(&set_backup, i));
        }
    }

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

    bitset_free(&set);

    printf("test_bitset_get_set: SUCCESS\n");
}

int main()
{
    test_bitset_allocate();
    test_bitset_get_set();
    return 0;
}
