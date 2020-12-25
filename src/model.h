#ifndef MODEL_H
#define MODEL_H

#include "common.h"

#include <stdint.h>
#include <raylib.h>
#include <raymath.h>

#define ENTITY_TYPES_COUNT 3

typedef enum
{
    PLAYER_ENTITY = 0,
    ENEMY_ENTITY = 1,
    TREE_ENTITY = 2,
    UNKNOWN_ENTITY = 1 << 31
} entity_type_t;

typedef enum
{
    GRASS_LAYER_TYPE,
    ROCK_LAYER_TYPE,
    SAND_LAYER_TYPE,
    PATH_LAYER_TYPE
} ground_layer_t;

#define EMPTY_ENTITY (entity_t) { .type =UNKNOWN_ENTITY, .id = -1  }
#define GET_PLAYER(model) (ent_player_t*)dynarr_get(&model->entities[PLAYER_ENTITY], 0)

typedef struct entity_t
{
    entity_type_t type;
    int id;
} entity_t;

typedef struct ent_player_t
{
    int mapid;
    uint16_t texture_id; // not move this position of this field
} ent_player_t;

typedef struct ent_enemy_t
{
    int mapid;
    uint16_t texture_id; // not move this position of this field
} ent_enemy_t;

typedef struct ent_tree_t
{
    int mapid;
    uint16_t texture_id; // not move this position of this field
} ent_tree_t;

typedef struct map_t
{
    uint16_t *ground_layer; // grass, sand, road etc
    uint16_t *ground_views;
    entity_t *entities;
    bitset_t obstacles;
    int width;
    int height;
}  map_t;

typedef struct model_t
{
    map_t map;
    dynarr_t entities[ENTITY_TYPES_COUNT];
} model_t;

int entity_get_mapid (const model_t* model, entity_t entity);
void entity_set_mapid (model_t* model, entity_t entity, int mapid);
int map_get_idx(const map_t * map, ivec_t pos);

com_result_t map_allocate(map_t * map, int width, int height)
{
    int size = width * height;

    map->ground_layer = malloc(size * sizeof *map->ground_layer);
    if (!map->ground_layer) return COM_ERR;

    map->ground_views = malloc(size * sizeof *map->ground_views);
    if (!map->ground_views) return COM_ERR;

    map->entities = malloc(size * sizeof *map->entities);
    if (!map->entities) return COM_ERR;

    map->obstacles = bitset_allocate(size);
    if (!map->obstacles.cells) return COM_ERR;

    map->width = width;
    map->height = height;

    return COM_OK;
}

int map_get_idx(const map_t * map, ivec_t pos)
{
    return pos.x + pos.y * map->width;
}

ivec_t map_get_pos(const map_t * map, int idx)
{
    return (ivec_t)
    {
        .x = idx % map->width,
        .y = idx / map->width
    };
}

// -1 - is invalid index
void map_get_neighbors(const map_t* map, ivec_t pos, int* neighbours_idx)
{

    map_get_idx(map, ivec_add(pos, (ivec_t){1, 0}));
}

void map_free (map_t * map)
{
    free(map->ground_layer);
    free(map->ground_views);
    free(map->entities);
    bitset_free(&map->obstacles);
}

void* add_entity(model_t* model, ivec_t pos, entity_type_t type)
{
    int idx = map_get_idx(&model->map, pos);
    dynarr_t *arr = &model->entities[type];
    dynarr_increment(arr);

    entity_t* e = &model->map.entities[idx];
    e->type = type;
    //e->id = model->entities[type].size - 1;
    e->id = arr->size - 1;
    entity_set_mapid(model, *e, idx);

    bitset_set(&model->map.obstacles, idx, 1);

    //return dynarr_get(arr, model->map.entities[idx].id);
    return dynarr_get(arr, e->id);
}

void remove_entity(model_t* model, map_t* map, ivec_t pos)
{
    int idx = map_get_idx(map, pos);
    entity_t entity = map->entities[idx];
    map->entities[idx] = EMPTY_ENTITY;
    bitset_set(&map->obstacles, idx, 0);

    dynarr_t *arr = &model->entities[entity.type];
    if (dynarr_remove_swap(arr, entity.id)) // if swap was present than TRUE
    {
        int mapid = entity_get_mapid(model, entity);
        map->entities[mapid].id = entity.id;
    }
}

// return 0 - not moved
// return 1 - moved
int move_entity(model_t* model, ivec_t from, ivec_t to)
{
    if (to.x < 0 || to.x >= model->map.width
        || to.y < 0 || to.y >= model->map.height) return 0;
    
    int idx_to = map_get_idx(&model->map, to);
    
    if (bitset_get(&model->map.obstacles, idx_to) != 0) return 0;

    int idx_from = map_get_idx(&model->map, from);

    entity_t entity = model->map.entities[idx_from];

    model->map.entities[idx_from] = EMPTY_ENTITY;
    model->map.entities[idx_to] = entity;
    bitset_set(&model->map.obstacles, idx_from, 0);
    bitset_set(&model->map.obstacles, idx_to, 1);

    entity_set_mapid(model, entity, idx_to);
    return 1;
}

int entity_get_mapid (const model_t* model, entity_t entity)
{
    return GET_AS(int, dynarr_get(&model->entities[entity.type], entity.id));
}

void entity_set_mapid (model_t* model, entity_t entity, int mapid)
{
    int* ptr = (int*)dynarr_get(&model->entities[entity.type], entity.id);
    *ptr = mapid;
}

uint16_t entity_get_texture_id (const model_t* model, entity_t entity)
{
    return GET_AS(uint16_t, (dynarr_get(&model->entities[entity.type], entity.id) + sizeof(int)));
}

static inline int manhatten_heuristic(ivec_t p1, ivec_t p2)
{
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

void reconstruct_path(const hashmap_t* path_links, int start_idx, int end_idx, dynarr_t* path)
{
    dynarr_add(path, &end_idx);
    int current = end_idx;
    int neighbore;
    while (current != start_idx)
    {
        assert(hashmap_get(path_links, current, &neighbore));
        current = neighbore;
        dynarr_add(path, &current);
    }
}

void find_path(ivec_t start, ivec_t end, const map_t* map, minheap_t* open_set, hashmap_t* path_links, hashmap_t* g_score, bitset_t* open_set_pops_tracker, dynarr_t* path)
{
    minheap_clear(open_set);
    dynarr_clear(path);
    hashmap_clear(path_links);
    bitset_clear(open_set_pops_tracker);

    int end_idx = map_get_idx(map, end);
    int start_idx = map_get_idx(map, start);

    minheap_push(open_set, (qnode_t){ manhatten_heuristic(start, start), start_idx});
    hashmap_add_or_replace(g_score, (qnode_t){start_idx, 0});

    qnode_t current;
    while(minheap_pop(open_set, &current))
    {
        // TODO: RECONSIDER THIS
        // NOT SURE ABOUT THIS CHECK
        // IT IS HERE BECAUSE WE NEED AN OPEN_SET,
        // THAT CONTAINS UNIQUE VALUES IN IT'S QNODE ELEMENTS
        // BUT IT ISN'T
        // SO WE FAKE THIS WITH ADDITIONAL BITSET OPEN_SET_POPS_TRACKER
        if(bitset_get(open_set_pops_tracker, current.value)) continue;
        bitset_set(open_set_pops_tracker, current.value, 1);

        if (current.value == end_idx) 
        {
            reconstruct_path(path_links, start_idx, end_idx, path);
            return;
        }

        int current_idx = current.value;
        ivec_t current_pos = map_get_pos(map, current_idx);

        ivec_t neighbors[4];
        {
            neighbors[0] = ivec_add(current_pos, (ivec_t){1, 0});
            neighbors[1] = ivec_add(current_pos, (ivec_t){-1, 0});
            neighbors[2] = ivec_add(current_pos, (ivec_t){0, 1});
            neighbors[3] = ivec_add(current_pos, (ivec_t){0, -1});
        }
        for (int i = 0; i < 4; i++)
        {
            ivec_t ngbr_pos = neighbors[i];
            if (ngbr_pos.x < 0 || ngbr_pos.x >= map->width 
                    || ngbr_pos.y < 0 || ngbr_pos.y >= map->height) continue;

            int ngbr_idx = map_get_idx(map, ngbr_pos);

            if (!bitset_get(&map->obstacles, ngbr_idx)) continue;

            int g_score_current_value;
            assert(hashmap_get(g_score, current_idx, &g_score_current_value));
            int tentative_score = g_score_current_value + 1;

            int g_score_ngbr_value;
            int g_score_result = hashmap_get(g_score, ngbr_idx, &g_score_ngbr_value);
            if (g_score_result == 0 || tentative_score < g_score_ngbr_value)
            {
                hashmap_add_or_replace(path_links, (qnode_t){ngbr_idx, current_idx});
                hashmap_add_or_replace(g_score, (qnode_t){ngbr_idx, tentative_score});

                int f_score = tentative_score + manhatten_heuristic(start, ngbr_pos);

                // TODO: RECONSIDER THIS
                // SEE FIRST TODO IN THIS FUNCTION
                minheap_push(open_set, (qnode_t){f_score, ngbr_idx});
            }
        }
    }
}

#endif // MODEL_H
