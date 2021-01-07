#ifndef GENERATOR_H
#define GENERATOR_H

#include "globals.h"
#include "common.h"
#include "model.h"
#include "simulation.h"
#include <stdlib.h>
#include <time.h>

typedef struct path_data_t
{
    bitset_t path_map;
    dynarr_t path_nodes;
} path_data_t;

typedef struct path_node_t
{
    ivec_t pos;
    uint8_t dirmask;
} path_node_t;

void generate_path_draw_line(const model_t* model, path_data_t* data,
        ivec_t p1, ivec_t p2, int is_vertical)
{
    ivec_t delta = ivec_sub(p2, p1);
    if (is_vertical)
    {
        assert(p1.x == p2.x);
        int start_y = delta.y > 0 ? p1.y : p2.y;
        int end_y = delta.y > 0 ? p2.y : p1.y;

        for (int y = start_y; y <= end_y; y++)
        {
            ivec_t current_pos = {p1.x, y};
            int current_idx = map_get_idx(&model->map, current_pos);
            bitset_set(&data->path_map, current_idx, 1);
        }
    }
    else // is horizontal
    {
        assert(p1.y == p2.y);
        int start_x = delta.x > 0 ? p1.x : p2.x;
        int end_x = delta.x > 0 ? p2.x : p1.x;

        for (int x = start_x; x <= end_x; x++)
        {
            ivec_t current_pos = {x, p1.y};
            int current_idx = map_get_idx(&model->map, current_pos);
            bitset_set(&data->path_map, current_idx, 1);
        }
    }
}

// TODO: finish all ifs
// TODO: add another paths
// TODO: add some offsets to first 4 generation
path_data_t generate_path_v1(model_t* model, int width, int height)
{
    path_data_t data;
    data.path_map = bitset_allocate(width * height);
    bitset_clear(&data.path_map);
    data.path_nodes = dynarr_allocate(sizeof(path_node_t), 0, 32);

    const path_node_t lu_corner = 
    {
        .pos = ivec_rand_from_range(IVEC_ZERO, (ivec_t){width/2 - 1, height/2 - 1}),
        .dirmask = MASK_BIT_DIR_LEFT | MASK_BIT_DIR_UP
    };
    const path_node_t rd_corner = 
    {
        .pos = ivec_rand_from_range((ivec_t){width/2, height/2}, (ivec_t){width - 1, height - 1}),
        .dirmask = MASK_BIT_DIR_RIGHT | MASK_BIT_DIR_DOWN
    };
    const path_node_t ld_corner = 
    {
        .pos = ivec_rand_from_range((ivec_t){0, height/2}, (ivec_t){width/2 - 1, height - 1}),
        .dirmask = MASK_BIT_DIR_LEFT | MASK_BIT_DIR_DOWN
    };
    const path_node_t ru_corner = 
    {
        .pos = ivec_rand_from_range((ivec_t){width/2, 0}, (ivec_t){width - 1, height/2 - 1}),
        .dirmask = MASK_BIT_DIR_RIGHT | MASK_BIT_DIR_UP
    };

    dynarr_add(&data.path_nodes, &lu_corner);
    dynarr_add(&data.path_nodes, &rd_corner);
    dynarr_add(&data.path_nodes, &ld_corner);
    dynarr_add(&data.path_nodes, &ru_corner);

    if (lu_corner.pos.y > ru_corner.pos.y)
    {
        int break_x = int_rand_range(lu_corner.pos.x, ru_corner.pos.x);
        const path_node_t up_node =
        {
            .pos = {break_x, lu_corner.pos.y},
            .dirmask = MASK_BIT_DIR_LEFT | MASK_BIT_DIR_DOWN
        };
        const path_node_t down_node =
        {
            .pos = {break_x, ru_corner.pos.y},
            .dirmask = MASK_BIT_DIR_RIGHT | MASK_BIT_DIR_UP
        };

        dynarr_add(&data.path_nodes, &up_node);
        dynarr_add(&data.path_nodes, &down_node);

        generate_path_draw_line(model, &data, lu_corner.pos, up_node.pos, 0);
        generate_path_draw_line(model, &data, up_node.pos, down_node.pos, 1);
        generate_path_draw_line(model, &data, down_node.pos, ru_corner.pos, 0);

    }
    else if (lu_corner.pos.x < ru_corner.pos.x)
    {

    }
    else
    {
        generate_path_draw_line(model, &data, lu_corner.pos, ru_corner.pos, 0);
    }

    if (lu_corner.pos.x == ld_corner.pos.x)
    {
    }
    if (rd_corner.pos.y == ld_corner.pos.y)
    {
    }
    if (rd_corner.pos.x == ru_corner.pos.x)
    {
    }



    return data;
}

void generate_location(int width, int height, model_t* model)
{
    time_t t;
    srand((unsigned) time(&t));

    model_allocate(model, width, height);
    map_t* map = &model->map;

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
        map->ground_views[idx] = TEXTURE_ID_PATHLINE_HORIZONTAL;
    }
    for (int y = 0; y < 11; y++)
    {
        int idx = map_get_idx(map, (ivec_t){11, y});
        map->ground_layer[idx] = PATH_LAYER_TYPE;
        map->ground_views[idx] = TEXTURE_ID_PATHLINE_VERICAL;
    }
    {
        int idx = map_get_idx(map, (ivec_t){11, 11});
        map->ground_layer[idx] = PATH_LAYER_TYPE;
        map->ground_views[idx] = TEXTURE_ID_PATHTURN_LU;
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
                player->health = 100;
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
                enemy->health = 50;
                break;
            }
        }
        if (find_place) break;
    }

    globals_allocate_per_location();
}

void destroy_location(model_t* model)
{
    globals_free_per_location();
    model_free(model);
    /*map_free(&model->map);
    for (int i = 0; i < ENTITY_TYPE_LENGTH; i++)
        dynarr_free(&model->entities[i]);*/
}

#endif // GENERATOR_H
