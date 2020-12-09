#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>

#define BITSET_CELL_BITCOUNT 32
#define BITSET_CELL_BYTECOUNT 4

typedef enum
{
    COM_OK = 0,
    COM_ERR = 1
} com_result_t;

typedef struct bitset_t
{
    uint32_t *cells;
    int cellcount; 
    int bitcount;
    int bytecount;
} bitset_t;


// stride - size of element in bytes
typedef struct dynarr_t
{
    void* data;
    int size;
    int capacity;
    int stride;
} dynarr_t;

bitset_t bitset_allocate(int bitcount)
{
    bitset_t set;
    set.cellcount =  bitcount / BITSET_CELL_BITCOUNT 
        + ((bitcount % BITSET_CELL_BITCOUNT) != 0);
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

dynarr_t dynarr_allocate(int stride, int size, int capacity)
{
    assert(stride > 0);
    dynarr_t arr;
    arr.stride = stride;
    arr.capacity = capacity > size ? capacity : size;
    if (arr.capacity == 0) arr.capacity = 4;
    arr.size = size;
    arr.data = malloc(arr.capacity * arr.stride);
    return arr;
}

void dynarr_add (dynarr_t* arr, void* elem)
{
    if (arr->capacity == arr->size)
    {
        arr->capacity = arr->capacity * 2;
        arr->data = realloc(arr->data, arr->capacity * arr->stride);
    }
    memcpy(arr->data + arr->size * arr->stride, elem, arr->stride);
    arr->size++;
}

void dynarr_remove_swap (dynarr_t* arr, int idx)
{
    assert(idx >= 0);
    assert(idx < arr->size);
    arr->size--; // last  element idx now equal arr->size
    if (arr->size > 0 && idx < arr->size)
    {
        void* ptr_idx = arr->data + idx * arr->stride;
        void* ptr_last = arr->data + arr->size * arr->stride;
        memcpy(ptr_idx, ptr_last, arr->stride);
    }
}

void* dynarr_get(const dynarr_t* arr, int idx)
{
    return arr->data + idx * arr->stride;
}

void dynarr_set(dynarr_t* arr, int idx, void* value)
{
    void* ptr = arr->data + idx * arr->stride;
    memcpy(ptr, value, arr->stride);
}

void dynarr_free(dynarr_t* arr)
{
    free(arr->data);
}

#define GET_AS(type, func) *(type*)func

#endif // COMMON_H
