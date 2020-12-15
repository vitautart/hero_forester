#ifndef GENERATOR_H
#define GENERATOR_H

#include "common.h"
#include "model.h"

#include "globals.h"

#include <stdlib.h>
#include <time.h>
#
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
    for (int y = 0; y < map->width; y++)
        for (int x = 0; x < map->height; x++)
            if (norm_rand() > 0.5)
                add_entity(container, map, (ivec_t) {x, y}, TREE_ENTITY);

    // ADD PLAYER TO RANDOM POSITION
    for (int y = 5; y < map->width - 5; y++)
        for (int x = 5; x < map->height - 5; x++)
            if (bitset_get(&map->obstacles, map_get_idx(map, (ivec_t){x, y}) == 0))
            {
                add_entity(container, map, (ivec_t) {x, y}, PLAYER_ENTITY);
                break;
            }

}

void destroy_location(entity_container_t* container, map_t* map)
{
    map_free(map);
    for (int i = 0; i < ENTITY_TYPES_COUNT; i++)
        dynarr_free(&container->entities[i]);
}

#endif // GENERATOR_H
