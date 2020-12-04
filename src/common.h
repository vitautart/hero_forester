#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <malloc.h>
#include <assert.h>

typedef struct bitset_t
{
    uint32_t *data;
    int data_size; 
    int elem_count;
    int elem_size_in_bits;
} bitset_t;

bitset_t bitset_allocate(int elem_count)
{
    bitset_t set;
    int size_in_bytes = sizeof *set.data;
    set.elem_size_in_bits = size_in_bytes * 8;

    set.data_size =  elem_count / set.elem_size_in_bits 
        + ((elem_count % set.elem_size_in_bits) != 0)* 1;
    set.data = calloc(set.data_size, size_in_bytes);
    set.elem_count = elem_count;
    return set;
}

void bitset_set(bitset_t* set, int idx, int value)
{
    value = value != 0;
    int data_idx = idx / set->elem_size_in_bits;
    int bit_pos = idx % set->elem_size_in_bits;
    set->data[data_idx] = (set->data[data_idx] & ~(1 << bit_pos)) | (value << bit_pos);
}

int bitset_get(const bitset_t* set, int idx)
{
    int data_idx = idx / set->elem_size_in_bits;
    int bit_pos = idx % set->elem_size_in_bits;
    return (set->data[data_idx] & (1 << bit_pos)) != 0;
}

void bitset_free(bitset_t *set)
{
    free(set->data);
}

#endif // COMMON_H
