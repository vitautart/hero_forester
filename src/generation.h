#ifndef GENERATOR_H
#define GENERATOR_H

#include "common.h"
#include "model.h"

#include "globals.h"

#include <stdlib.h>
#include <time.h>

float norm_rand()
{
    return (RAND_MAX - rand()) / (float)RAND_MAX;
}


void generate_location(int width, int height, entity_container_t* container, map_t* map)
{
    map_allocate(map, width, height);
    container->entities[PLAYER_ENTITY] = dynarr_allocate(sizeof(ent_player_t), 0, 1);
    container->entities[ENEMY_ENTITY] = dynarr_allocate(sizeof(ent_enemy_t), 0, 32);
    container->entities[TREE_ENTITY] = dynarr_allocate(sizeof(ent_enemy_t), 0, width * height);


    // GENERATION OF TREES
    time_t t;
    srand((unsigned) time(&t));

    // FILL MAP GROUND LAYER
    for (int y = 0; y < map->height; y++)
        for (int x = 0; x < map->width; x++)
        {
            int idx = map_get_idx(map, (ivec_t){x, y});
            map->ground_layer[idx] = GRASS_LAYER_TYPE;
            map->ground_views[idx] = TEXTURE_ID_GRASS_1;
        }

    // SET OBSTACLES TO ZERO
    for (int i = 0; i < map->height * map->width; i++)
        bitset_set(&map->obstacles, i, 0);

    // GENERATION OF TREES AND FILL EMPTY ENTITIES
    for (int y = 0; y < map->height; y++)
        for (int x = 0; x < map->width; x++)
            if (norm_rand() > 0.5)
            {
                ent_tree_t* tree = (ent_tree_t*)add_entity(container, map, (ivec_t) {x, y}, TREE_ENTITY);
                tree->texture_id = TEXTURE_ID_TREE_1;
            }
            else
            {
                map->entities[map_get_idx(map, (ivec_t){x, y})] = EMPTY_ENTITY;
            }

    // ADD PLAYER TO RANDOM POSITION
    for (int y = 5; y < (map->height - 5); y++)
    {
        int find_place = 0;
        for (int x = 5; x < (map->width - 5); x++)
        {
            find_place = bitset_get(&map->obstacles, map_get_idx(map, (ivec_t){x, y})) == 0;
            if (find_place)
            {
                ent_player_t* player = (ent_player_t*)add_entity(container, map, (ivec_t) {x, y}, PLAYER_ENTITY);
                player->texture_id = TEXTURE_ID_PLAYER_2;
                break;
            }
        }
        if (find_place) break;
    }
}

void destroy_location(entity_container_t* container, map_t* map)
{
    map_free(map);
    for (int i = 0; i < ENTITY_TYPES_COUNT; i++)
        dynarr_free(&container->entities[i]);
}

#endif // GENERATOR_H
