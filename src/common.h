#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

#define BITSET_CELL_BITCOUNT 32
#define BITSET_CELL_BYTECOUNT 4
#define GET_AS(type, func) *(type*)func

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

typedef struct qnode_t
{
    int key;
    int value;
} qnode_t;

typedef struct minheap_t
{
    dynarr_t data;
} minheap_t;

typedef struct hashmap_t
{
    dynarr_t* buckets;
    uint32_t bucket_count;
    int size;
} hashmap_t;

typedef struct ivec_t
{
    int x;
    int y;
} ivec_t;

ivec_t ivec_add(ivec_t v1, ivec_t v2)
{
    return (ivec_t) 
    {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y
    };
}

ivec_t ivec_mul(int s, ivec_t v)
{
    return (ivec_t)
    {
        .x = s * v.x,
        .y = s * v.y
    };
}

ivec_t ivec_sub(ivec_t v1, ivec_t v2)
{
    return (ivec_t) 
    {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y
    };
}

float norm_rand()
{
    return (RAND_MAX - rand()) / (float)RAND_MAX;
}

uint32_t knuth_mult_hash(uint32_t input)
{
    return (input * 2654435761) % UINT_MAX;
}

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
    arr.size = size;
    arr.stride = stride;
    arr.capacity = capacity > size ? capacity : size;
    arr.capacity = arr.capacity != 0 ? arr.capacity : 4;
    arr.data = malloc(arr.capacity * arr.stride);
    return arr;
}

com_result_t dynarr_add (dynarr_t* arr, void* elem)
{
    if (arr->capacity == arr->size)
    {
        arr->capacity = arr->capacity * 2;
        void* ptr = realloc(arr->data, arr->capacity * arr->stride);
        if (ptr == NULL)
            return COM_ERR;
        arr->data = ptr;
    }
    memcpy(arr->data + arr->size * arr->stride, elem, arr->stride);
    arr->size++;
    return COM_OK;
}

com_result_t dynarr_increment (dynarr_t* arr)
{
    if (arr->capacity == arr->size)
    {
        arr->capacity = arr->capacity * 2;
        void* ptr = realloc(arr->data, arr->capacity * arr->stride);
        if (ptr == NULL)
            return COM_ERR;
        arr->data = ptr;
    }
    arr->size++;
    return COM_OK;
}

// return 1 - swap was happened
// return 0 - swap wasn't happened
int dynarr_remove_swap(dynarr_t* arr, int idx)
{
    assert(idx >= 0);
    assert(idx < arr->size);
    arr->size--; // last  element idx now equal arr->size
    if (arr->size > 0 && idx < arr->size)
    {
        void* ptr_idx = arr->data + idx * arr->stride;
        void* ptr_last = arr->data + arr->size * arr->stride;
        memcpy(ptr_idx, ptr_last, arr->stride);
        return 1;
    }
    return 0;
}



void dynarr_remove(dynarr_t* arr, int idx)
{
    assert(idx >= 0);
    assert(idx < arr->size);
    arr->size--; 
    if(arr->size > 0 && idx < arr->size)
    {
        void* dest = arr->data + idx * arr->stride;
        int next_idx = idx + 1;
        void* src = arr->data + next_idx * arr->stride;
        int bytecount = (arr->size - next_idx + 1) * arr->stride;
        memmove(dest, src, bytecount);
    }
}

void dynarr_set(dynarr_t* arr, int idx, void* value)
{
    void* ptr = arr->data + idx * arr->stride;
    memcpy(ptr, value, arr->stride);
}

void* dynarr_get(const dynarr_t* arr, int idx)
{
    return arr->data + idx * arr->stride;
}

void dynarr_swap(dynarr_t* arr, int idx_1, int idx_2)
{
    char temp[arr->stride];
    void* ptr_1 = dynarr_get(arr, idx_1);
    void* ptr_2 = dynarr_get(arr, idx_2);
    memcpy(temp, ptr_1, arr->stride);
    memcpy(ptr_1, ptr_2, arr->stride);
    memcpy(ptr_2, temp, arr->stride);
}

void dynarr_free(dynarr_t* arr)
{
    free(arr->data);
}

int minheap_get_child_id_1(int i)
{
    return 2 * i + 1;
}

int minheap_get_child_id_2(int i)
{
    return 2 * i + 2;
}

int minheap_get_parent_id(int i)
{
    return (int)floor( (i - 1) * 0.5f ); 
}

minheap_t minheap_allocate(int capacity)
{
    minheap_t queue;
    queue.data = dynarr_allocate(sizeof(qnode_t), 0, capacity);
    return queue;
}

void minheap_free(minheap_t* queue)
{
    dynarr_free(&queue->data);
}

com_result_t minheap_push(minheap_t* queue, qnode_t node)
{
    dynarr_t* data = &queue->data;
    com_result_t result = dynarr_add(data, &node);

    if (result == COM_ERR) return result;

    int current_idx = data->size - 1;
    while(1)
    {
        if (current_idx == 0) break;
        int current_parent_idx = minheap_get_parent_id(current_idx);
        qnode_t* parent = (qnode_t*)dynarr_get(data, current_parent_idx);
        if (parent->key <= node.key) break;
        dynarr_swap(data, current_idx, current_parent_idx);
        current_idx = current_parent_idx;
    }

    return COM_OK;
}

int minheap_pop(minheap_t* queue, qnode_t* node)
{
    dynarr_t* data = &queue->data;
    if (data->size == 0) return 0;
    *node = GET_AS(qnode_t, dynarr_get(data, 0));
    if (!dynarr_remove_swap(data, 0)) return 1;

    int current_parent_idx = 0;
    while(1)
    {
        int child_idx_1 = minheap_get_child_id_1(current_parent_idx);
        int child_idx_2 = minheap_get_child_id_2(current_parent_idx);
        if (child_idx_1 >= data->size || child_idx_2 >= data->size) break;
        qnode_t* child_1 = (qnode_t*)dynarr_get(data, child_idx_1);
        qnode_t* child_2 = (qnode_t*)dynarr_get(data, child_idx_2);
        int child_idx_to_swap = child_1->key < child_2->key ? child_idx_1 : child_idx_2;
        dynarr_swap(data, child_idx_to_swap, current_parent_idx);
        current_parent_idx = child_idx_to_swap;
    }

    return 1;
}

hashmap_t hashmap_allocate(uint32_t bucket_count, int bucket_capacity)
{
    hashmap_t map;
    map.size = 0;
    map.bucket_count = bucket_count;
    map.buckets = malloc(map.bucket_count * sizeof(dynarr_t));

    for(int i = 0; i < map.bucket_count; i++)
        map.buckets[i] = dynarr_allocate(sizeof(qnode_t), 0, bucket_capacity);

    return map;
}

void hashmap_free(hashmap_t* map)
{
    for(int i = 0; i < map->bucket_count; i++)
        dynarr_free(&map->buckets[i]);

    free(map->buckets);
}

void hashmap_add_or_replace(hashmap_t* map, qnode_t node)
{
    uint32_t id = knuth_mult_hash(node.key) % map->size;
    dynarr_t* bucket = &map->buckets[id];

    map->size++; // MAP SIZE CHANGED !!!!
    for (int i = 0; i < bucket->size; i++)
    {
        qnode_t* current = (qnode_t*)dynarr_get(bucket, i);
        if (current->key == node.key)
        {
            dynarr_set(bucket, i, &node);
            return;
        }
    }

    dynarr_add(bucket, &node);
}

// 0 - not added
// 1 - added
int hashmap_try_add(hashmap_t* map, qnode_t node)
{   
    uint32_t id = knuth_mult_hash(node.key) % map->size;
    dynarr_t* bucket = &map->buckets[id];

    for (int i = 0; i < bucket->size; i++)
    {
        qnode_t* current = (qnode_t*)dynarr_get(bucket, i);
        if (current->key == node.key) return 0;
    }

    dynarr_add(bucket, &node);
    map->size++;
    return 1;
}

// 0 - key wasn't fount 
// 1 - key was found
int hashmap_get(hashmap_t* map, int key, int* value)
{
    uint32_t id = knuth_mult_hash(key) % map->size;
    dynarr_t* bucket = &map->buckets[id];

    for (int i = 0; i < bucket->size; i++)
    {
        qnode_t* current = (qnode_t*)dynarr_get(bucket, i);
        if (current->key == key)
        {
            *value = current->value;
            return 1;
        }
    }

    return 0;
}

// 0 - key wasn't found
// 1 - key was found, and value removed
int hashmap_remove(hashmap_t* map, int key)
{
    uint32_t id = knuth_mult_hash(key) % map->size;
    dynarr_t* bucket = &map->buckets[id];

    for (int i = 0; i < bucket->size; i++)
    {
        qnode_t* current = (qnode_t*)dynarr_get(bucket, i);
        if (current->key == key) 
        {
            dynarr_remove_swap(bucket, i);
            map->size--;
            return 1;
        }
    }

    return 0;
}

#endif // COMMON_H
