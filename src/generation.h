#ifndef GENERATOR_H
#define GENERATOR_H

#include "globals.h"
#include "common.h"
#include "model.h"
#include "simulation.h"
#include <stdlib.h>
#include <time.h>

void globals_allocate(const map_t* map)
{
    global_open_set = minheap_allocate(10000);
    global_path_links = hashmap_allocate(128, 64);
    global_g_score = hashmap_allocate(128, 64);
    global_path = dynarr_allocate(sizeof(qnode_t), 0, 128);
#ifdef DEBUG_RENDER
    global_debug_red_map_cell_pos = dynarr_allocate(sizeof(ivec_t), 0, 128);
    global_debug_blue_map_cell_pos = dynarr_allocate(sizeof(ivec_t), 0, 128);
    global_debug_grey_map_cell_pos = dynarr_allocate(sizeof(ivec_t), 0, 128);
    global_debug_yellow_map_cell_pos = dynarr_allocate(sizeof(ivec_t), 0, 128);
#endif
}

void globals_free()
{
    minheap_free(&global_open_set);
    hashmap_free(&global_path_links);
    hashmap_free(&global_g_score);
    dynarr_free(&global_path);
#ifdef DEBUG_RENDER
    dynarr_free(&global_debug_red_map_cell_pos);
    dynarr_free(&global_debug_blue_map_cell_pos);
    dynarr_free(&global_debug_grey_map_cell_pos);
    dynarr_free(&global_debug_yellow_map_cell_pos);
#endif
}

void generate_location(int width, int height, model_t* model)
{
    map_t* map = &model->map;
    map_allocate(&model->map, width, height);
    model->entities[PLAYER_ENTITY] = dynarr_allocate(sizeof(ent_player_t), 0, 1);
    model->entities[ENEMY_ENTITY] = dynarr_allocate(sizeof(ent_enemy_t), 0, 32);
    model->entities[TREE_ENTITY] = dynarr_allocate(sizeof(ent_enemy_t), 0, width * height);

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

    // ADD PATHS
    for (int x = 0; x < 11; x++)
    {
        int idx = map_get_idx(map, (ivec_t){x, 11});
        map->ground_layer[idx] = PATH_LAYER_TYPE;
        map->ground_views[idx] = TEXTURE_ID_PATHLINE_1;
    }
    for (int y = 0; y < 11; y++)
    {
        int idx = map_get_idx(map, (ivec_t){11, y});
        map->ground_layer[idx] = PATH_LAYER_TYPE;
        map->ground_views[idx] = TEXTURE_ID_PATHLINE_2;
    }
    {
        int idx = map_get_idx(map, (ivec_t){11, 11});
        map->ground_layer[idx] = PATH_LAYER_TYPE;
        map->ground_views[idx] = TEXTURE_ID_PATHTURN_1;
    }

    // SET OBSTACLES TO ZERO
    for (int i = 0; i < map->height * map->width; i++)
        bitset_set(&map->obstacles, i, 0);

    // GENERATION OF TREES AND FILL EMPTY ENTITIES
    for (int y = 0; y < map->height; y++)
        for (int x = 0; x < map->width; x++)
            if (norm_rand() > 0.5)
            {
                ent_tree_t* tree = (ent_tree_t*)add_entity(model, (ivec_t) {x, y}, TREE_ENTITY);
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
                ent_player_t* player = (ent_player_t*)add_entity(model, (ivec_t) {x, y}, PLAYER_ENTITY);
                player->texture_id = TEXTURE_ID_PLAYER_2;
                break;
            }
        }
        if (find_place) break;
    }

    // TODO: add more enemy
    // ADD ENEMY TO RANDOM POSITION
    for (int y = 5; y < (map->height - 5); y++)
    {
        int find_place = 0;
        for (int x = 5; x < (map->width - 5); x++)
        {
            find_place = bitset_get(&map->obstacles, map_get_idx(map, (ivec_t){x, y})) == 0;
            if (find_place)
            {
                ent_enemy_t* enemy = (ent_enemy_t*)add_entity(model, (ivec_t) {x, y}, ENEMY_ENTITY);
                enemy->texture_id = TEXTURE_ID_ENEMY_1;
                break;
            }
        }
        if (find_place) break;
    }

    globals_allocate(&model->map);
}

void destroy_location(model_t* model)
{
    globals_free();
    map_free(&model->map);
    for (int i = 0; i < ENTITY_TYPES_COUNT; i++)
        dynarr_free(&model->entities[i]);
}

#endif // GENERATOR_H
