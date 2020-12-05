#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <malloc.h>
#include <assert.h>

#define BITSET_CELL_BITCOUNT 32
#define BITSET_CELL_BYTECOUNT 4

typedef struct bitset_t
{
    uint32_t *cells;
    int cellcount; 
    int bitcount;
    int bytecount;
} bitset_t;

bitset_t bitset_allocate(int bitcount)
{
    bitset_t set;
    set.cellcount =  bitcount / BITSET_CELL_BITCOUNT 
        + ((bitcount % BITSET_CELL_BITCOUNT) != 0)* 1;
    set.bytecount = set.cellcount * BITSET_CELL_BYTECOUNT;
    set.cells = malloc(set.bytecount);
    set.bitcount = bitcount;
    return set;
}

void bitset_set(bitset_t* set, int idx, int value)
{
    value = value != 0;
    int data_idx = idx / BITSET_CELL_BITCOUNT;
    int bit_pos = idx % BITSET_CELL_BITCOUNT;
    set->cells[data_idx] = (set->cells[data_idx] & ~(1 << bit_pos)) | (value << bit_pos);
}

int bitset_get(const bitset_t* set, int idx)
{
    int data_idx = idx / BITSET_CELL_BITCOUNT;
    int bit_pos = idx % BITSET_CELL_BITCOUNT;
    return (set->cells[data_idx] & (1 << bit_pos)) != 0;
}

void bitset_free(bitset_t *set)
{
    free(set->cells);
}

#endif // COMMON_H
