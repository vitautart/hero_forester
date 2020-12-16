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
    SAND_LAYER_TYPE
} ground_layer_t;

#define EMPTY_ENTITY (entity_t) { .type =UNKNOWN_ENTITY, .id = -1  }

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

typedef struct entity_container_t
{ 
    dynarr_t entities[ENTITY_TYPES_COUNT];
} entity_container_t;

typedef struct map_t
{
    uint16_t *ground_layer; // grass, sand, road etc
    uint16_t *ground_views;
    entity_t *entities;
    bitset_t obstacles;
    int width;
    int height;
}  map_t;


int entity_get_mapid (const entity_container_t* entities, entity_t entity);
void entity_set_mapid (const entity_container_t* entities, entity_t entity, int mapid);
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

    for (int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
        {

            map->entities[map_get_idx(map, (ivec_t){x, y})] = EMPTY_ENTITY;
        }

    return COM_OK;
}

int map_get_idx(const map_t * map, ivec_t pos)
{
    return pos.x + pos.y * map->width;
}

void map_free (map_t * map)
{
    free(map->ground_layer);
    free(map->ground_views);
    free(map->entities);
    bitset_free(&map->obstacles);
}

void* add_entity(entity_container_t* container, map_t* map, ivec_t pos, entity_type_t type)
{
    int idx = map_get_idx(map, pos);
    dynarr_t *arr = &container->entities[type];
    dynarr_add(arr, &(ent_player_t){.mapid = idx});
    map->entities[idx].type = TREE_ENTITY;
    map->entities[idx].id = container->entities[PLAYER_ENTITY].size - 1;
    bitset_set(&map->obstacles, idx, 1);
    return dynarr_get(arr, map->entities[idx].id);
}

void remove_entity(entity_container_t* container, map_t* map, ivec_t pos)
{
    int idx = map_get_idx(map, pos);
    entity_t entity = map->entities[idx];
    map->entities[idx] = EMPTY_ENTITY;
    bitset_set(&map->obstacles, idx, 0);

    dynarr_t *arr = &container->entities[entity.type];
    if (dynarr_remove_swap(arr, entity.id)) // if swap was present than TRUE
    {
        int mapid = entity_get_mapid(container, entity);
        map->entities[mapid].id = entity.id;
    }
}

void move_entity(entity_container_t* container, map_t* map, ivec_t from, ivec_t to)
{
    int idx_to = map_get_idx(map, to);

    if (bitset_get(&map->obstacles, idx_to) != 0) return;

    int idx_from = map_get_idx(map, from);

    entity_t entity = map->entities[idx_from];
    map->entities[idx_from] = EMPTY_ENTITY;
    map->entities[idx_to] = entity;
    bitset_set(&map->obstacles, idx_from, 0);
    bitset_set(&map->obstacles, idx_to, 1);

    entity_set_mapid(container, entity, idx_to);
}


int entity_get_mapid (const entity_container_t* entities, entity_t entity)
{
    return GET_AS(int, dynarr_get(&entities->entities[entity.type], entity.id));
}

void entity_set_mapid (const entity_container_t* entities, entity_t entity, int mapid)
{
    int* ptr = (int*)dynarr_get(&entities->entities[entity.type], entity.id);
    *ptr = mapid;
}

uint16_t entity_get_texture_id (const entity_container_t* entities, entity_t entity)
{
    return GET_AS(uint16_t, (dynarr_get(&entities->entities[entity.type], entity.id) + sizeof(int)));
}

#endif // MODEL_H
