#ifndef MODEL_H
#define MODEL_H

#include "common.h"

#include <stdint.h>
#include <raylib.h>

typedef enum
{
    UNKNOWN,
    PLAYER,
    ENEMY,
    TREE
} entity_type_t;

typedef struct entity_t
{
    entity_type_t type;
    int id;
} entity_t;

typedef struct ent_player_t
{
    int mapid;
} ent_player_t;

typedef struct ent_enemy_t
{
    int mapid;
} ent_enemy_t;

typedef struct ent_tree_t
{
    int mapid;
} ent_tree_t;

typedef struct entity_manager_t
{ 
    dynarr_t players; // ent_player_t
    dynarr_t enemies; // ent_enemy_t
    dynarr_t trees; // ent_tree_t
} entity_manager_t;

typedef struct map_t
{
    uint16_t *ground_layer; // grass, sand, road etc
    uint16_t *ground_views;
    entity_t *entities;
    bitset_t obstacles;
    int width;
    int height;
}  map_t;

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
#endif // MODEL_H
