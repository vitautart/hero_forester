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

#endif // MODEL_H
