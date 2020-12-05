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

    printf("test_bitset_allocate: SUCCESS\n");
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

    printf("test_bitset_get_set: SUCCESS\n");
}

int main()
{
    test_bitset_allocate();
    test_bitset_get_set();
    return 0;
}
