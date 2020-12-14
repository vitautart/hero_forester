#ifndef MODEL_H
#define MODEL_H

#include "common.h"

#include <stdint.h>
#include <raylib.h>

typedef enum
{
    PLAYER_ENTITY = 0,
    ENEMY_ENTITY = 1,
    TREE_ENTITY = 2,
    UNKNOWN_ENTITY = 1 << 31
} entity_type_t;

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
    dynarr_t entities[3];
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

inline int map_get_idx(const map_t * map, int x, int y)
{
    return x + y * map->width;
}

void map_free (map_t * map)
{
    free(map->ground_layer);
    free(map->ground_views);
    free(map->entities);
    bitset_free(&map->obstacles);
}

void* add_entity(entity_container_t* container, map_t* map, int x, int y, entity_type_t type)
{
    int idx = map_get_idx(map, x, y);
    dynarr_t *arr = &container->entities[type];
    dynarr_add(arr, &(ent_player_t){.mapid = idx});
    map->entities[idx].type = TREE_ENTITY;
    map->entities[idx].id = container->entities[PLAYER_ENTITY].size - 1;
    return dynarr_get(arr, map->entities[idx].id);
}

void remove_entity(entity_container_t* container, map_t* map, int x, int y)
{
    int idx = map_get_idx(map, x, y);
    entity_t entity = map->entities[idx];
    dynarr_t *arr = &container->entities[entity.type];
    dynarr_remove_swap(arr, entity.id);
    int mapid = entity_get_mapid(container, entity);
    map->entities[mapid].id = idx;
}

int entity_get_mapid (const entity_container_t* entities, entity_t entity)
{
    return GET_AS(int, dynarr_get(&entities->entities[entity.type], entity.id));
}

uint16_t entity_get_texture_id (const entity_container_t* entities, entity_t entity)
{
    return GET_AS(uint16_t, (dynarr_get(&entities->entities[entity.type], entity.id) + sizeof(int)));
}

#endif // MODEL_H
