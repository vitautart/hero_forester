#ifndef COMMON_H
#define COMMON_H

#define UNIT_TEST

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
    set->data[data_idx] = (set->data[data_idx] & !(1 << bit_pos)) | (value << bit_pos);
}

int bitset_get(const bitset_t* set, int idx)
{
    int data_idx = idx / set->elem_size_in_bits;
    int bit_pos = idx % set->elem_size_in_bits;
    return set->data[data_idx] & (1 << bit_pos);
}

void bitset_free(bitset_t *set)
{
    free(set->data);
}

#ifdef UNIT_TEST

void test_bitset_allocate()
{
    bitset_t set = bitset_allocate(65);
    
    assert(set.data != NULL);
    assert(set.data_size == 3);
    assert(set.elem_size_in_bits == 32);
    assert(set.elem_count == 65);
    for (int i = 0; i < set.data_size; i++)
    {
        assert(set.data[i] == 0);
    }
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
    
    int idx = 19;
    bitset_set(&set, idx, 2);
    assert(bitset_get(&set, idx) != 0);
    bitset_set(&set, idx, 0);
    assert(bitset_get(&set, idx) == 0);

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
#endif

#endif // COMMON_H
