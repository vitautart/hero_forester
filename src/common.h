#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"
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
#define IVEC_ZERO (ivec_t){0,0}

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

typedef struct iaabb_t
{
    ivec_t min;
    ivec_t max;
} iaabb_t;

static inline float norm_rand();
static inline uint32_t knuth_mult_hash(uint32_t input);

// ivec_t functions
static inline ivec_t ivec_add(ivec_t v1, ivec_t v2);
static inline ivec_t ivec_mul(int s, ivec_t v);
static inline ivec_t ivec_sub(ivec_t v1, ivec_t v2);
static inline ivec_t ivec_clamp(ivec_t v, ivec_t min, ivec_t max);
static inline ivec_t ivec_mul_v2(ivec_t vi, Vector2 vf);

// bitset_t functions
bitset_t bitset_allocate(int bitcount);
void bitset_set(bitset_t* set, int idx, int value);
int bitset_get(const bitset_t* set, int idx);
void bitset_clear(bitset_t* set);
void bitset_free(bitset_t *set);

// dynarr_t functions
dynarr_t dynarr_allocate(int stride, int size, int capacity);
com_result_t dynarr_add (dynarr_t* arr, const void* elem);
com_result_t dynarr_increment (dynarr_t* arr);
// return 1 - swap was happened
// return 0 - swap wasn't happened
int dynarr_remove_swap(dynarr_t* arr, int idx);
void dynarr_remove(dynarr_t* arr, int idx);
void dynarr_set(dynarr_t* arr, int idx, void* value);
void* dynarr_get(const dynarr_t* arr, int idx);
void* dynarr_get_last(const dynarr_t* arr);
void dynarr_swap(dynarr_t* arr, int idx_1, int idx_2);
void dynarr_clear(dynarr_t* arr);
void dynarr_free(dynarr_t* arr);

// minheap_t functions
static inline int minheap_get_child_id_1(int i);
static inline int minheap_get_child_id_2(int i);
static inline int minheap_get_parent_id(int i);
minheap_t minheap_allocate(int capacity);
void minheap_free(minheap_t* queue);
void minheap_clear(minheap_t* queue);
com_result_t minheap_push(minheap_t* queue, qnode_t node);
int minheap_pop(minheap_t* queue, qnode_t* node);

// hashmap_t functions
hashmap_t hashmap_allocate(uint32_t bucket_count, int bucket_capacity);
void hashmap_free(hashmap_t* map);
void hashmap_clear(hashmap_t* map);
dynarr_t* hashmap_get_bucket(const hashmap_t* map, uint32_t key);
void hashmap_add_or_replace(hashmap_t* map, qnode_t node);
// 0 - not added
// 1 - added
int hashmap_try_add(hashmap_t* map, qnode_t node);
// 0 - key wasn't fount 
// 1 - key was found
int hashmap_get(const hashmap_t* map, int key, int* value);
// 0 - key wasn't found
// 1 - key was found, and value removed
int hashmap_remove(hashmap_t* map, int key);


static inline float norm_rand()
{
    return (RAND_MAX - rand()) / (float)RAND_MAX;
}

static inline int int_rand_range(int min, int max)
{
    return min + (max - min) * norm_rand();
}

static inline ivec_t ivec_rand_from_range(ivec_t min, ivec_t max)
{
    ivec_t delta = ivec_sub(max, min);
    delta.x = (int)(norm_rand() * delta.x);
    delta.y = (int)(norm_rand() * delta.y);
    return ivec_add(min, delta);
}

static inline uint32_t knuth_mult_hash(uint32_t input)
{
    return (input * 2654435761) % UINT_MAX;
}

static inline ivec_t ivec_add(ivec_t v1, ivec_t v2)
{
    return (ivec_t) 
    {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y
    };
}

static inline ivec_t ivec_mul(int s, ivec_t v)
{
    return (ivec_t)
    {
        .x = s * v.x,
        .y = s * v.y
    };
}

static inline ivec_t ivec_sub(ivec_t v1, ivec_t v2)
{
    return (ivec_t) 
    {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y
    };
}

static inline ivec_t ivec_clamp(ivec_t v, ivec_t min, ivec_t max)
{
    return (ivec_t)
    {
        .x = v.x > min.x ? v.x < max.x ? v.x : max.x : min.x,
        .y = v.y > min.y ? v.y < max.y ? v.y : max.y : min.y
    };
}

static inline ivec_t ivec_mul_v2(ivec_t vi, Vector2 vf)
{
    return (ivec_t)
    {
        .x = (float)vi.x * vf.x,
        .y = (float)vi.y * vf.y
    };
}
static inline int minheap_get_child_id_1(int i)
{
    return 2 * i + 1;
}

static inline int minheap_get_child_id_2(int i)
{
    return 2 * i + 2;
}

static inline int minheap_get_parent_id(int i)
{
    return (int)floor( (i - 1) * 0.5f ); 
}

#endif // COMMON_H
