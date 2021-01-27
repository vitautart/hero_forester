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

void generate_path_draw_zigzag(const model_t*model, path_data_t* data, ivec_t p1, ivec_t p2, int is_vertical)
{
    if (is_vertical)
    {
        ivec_t up = p1.y < p2.y ? p1 : p2;
        ivec_t down = p1.y < p2.y ? p2 : p1;
        if (up.x == down.x)
        {
            generate_path_draw_line(model, data, up, down, 1);
        }
        else
        {
            int is_up_to_left = up.x < down.x;
            int break_y = int_rand_range(up.y + 2, down.y - 2);
            const path_node_t up_oposite_node =
            {
                .pos = {up.x, break_y},
                .dirmask = MASK_BIT_DIR_UP | (is_up_to_left ? MASK_BIT_DIR_RIGHT : MASK_BIT_DIR_LEFT)
            };
            const path_node_t down_oposite_node =
            {
                .pos = {down.x, break_y},
                .dirmask = MASK_BIT_DIR_DOWN | (is_up_to_left ? MASK_BIT_DIR_LEFT : MASK_BIT_DIR_RIGHT)        
            };

            dynarr_add(&data->path_nodes, &up_oposite_node);
            dynarr_add(&data->path_nodes, &down_oposite_node);

            generate_path_draw_line(model, data, up, up_oposite_node.pos, 1);
            generate_path_draw_line(model, data, up_oposite_node.pos, down_oposite_node.pos, 0);
            generate_path_draw_line(model, data, down_oposite_node.pos, down, 1);
        }
    }
    else {
        ivec_t left = p1.x < p2.x ? p1 : p2;
        ivec_t right = p1.x < p2.x ? p2 : p1;
        if (left.y == right.y)
        {
            generate_path_draw_line(model, data, left, right, 0);
        }
        else
        {
            int is_left_to_up = left.y < right.y;
            int break_x = int_rand_range(left.x + 2, right.x - 2);
            const path_node_t left_oposite_node =
            {
                .pos = {break_x, left.y},
                .dirmask = MASK_BIT_DIR_LEFT | (is_left_to_up ? MASK_BIT_DIR_DOWN : MASK_BIT_DIR_UP)
            };
            const path_node_t right_oposite_node =
            {
                .pos = {break_x, right.y},
                .dirmask = MASK_BIT_DIR_RIGHT | (is_left_to_up ? MASK_BIT_DIR_UP : MASK_BIT_DIR_DOWN)
            };

            dynarr_add(&data->path_nodes, &left_oposite_node);
            dynarr_add(&data->path_nodes, &right_oposite_node);

            generate_path_draw_line(model, data, left, left_oposite_node.pos, 0);
            generate_path_draw_line(model, data, left_oposite_node.pos, right_oposite_node.pos, 1);
            generate_path_draw_line(model, data, right_oposite_node.pos, right, 0);
        }
    }
}

// TODO: add another paths
// TODO: add some offsets to first 4 generation
path_data_t generate_path_v1(model_t* model)
{
    int c_offset = 5;
    int b_offset = 5;
    int width = model->map.width;
    int height = model->map.height;
    path_data_t data;
    data.path_map = bitset_allocate(width * height);
    bitset_clear(&data.path_map);
    data.path_nodes = dynarr_allocate(sizeof(path_node_t), 0, 32);

    const path_node_t lu_corner = 
    {
        .pos = ivec_rand_from_range((ivec_t){b_offset, b_offset}, (ivec_t){width/2 - 1 - c_offset, height/2 - 1 - c_offset}),
        .dirmask = MASK_DIR_RD
    };
    const path_node_t rd_corner = 
    {
        .pos = ivec_rand_from_range((ivec_t){width/2 + c_offset, height/2 + c_offset}, (ivec_t){width - 1 - b_offset, height - 1 - b_offset}),
        .dirmask = MASK_DIR_LU
    };
    const path_node_t ld_corner = 
    {
        .pos = ivec_rand_from_range((ivec_t){b_offset, height/2 + c_offset}, (ivec_t){width/2 - 1 - c_offset, height - 1 - b_offset}),
        .dirmask = MASK_DIR_RU
    };
    const path_node_t ru_corner = 
    {
        .pos = ivec_rand_from_range((ivec_t){width/2 + c_offset,  b_offset}, (ivec_t){width - 1 - b_offset, height/2 - 1 - c_offset}),
        .dirmask = MASK_DIR_LD
    };

    dynarr_add(&data.path_nodes, &lu_corner);
    dynarr_add(&data.path_nodes, &rd_corner);
    dynarr_add(&data.path_nodes, &ld_corner);
    dynarr_add(&data.path_nodes, &ru_corner);

    generate_path_draw_zigzag(model, &data, lu_corner.pos, ru_corner.pos, 0);
    generate_path_draw_zigzag(model, &data, ld_corner.pos, rd_corner.pos, 0);
    generate_path_draw_zigzag(model, &data, lu_corner.pos, ld_corner.pos, 1);
    generate_path_draw_zigzag(model, &data, ru_corner.pos, rd_corner.pos, 1);

    return data;
}

void generate_ground_layer_v1(const path_data_t* path, model_t* model)
{
    map_t* map = &model->map;

    for (int y = 0; y < map->height; y++)
        for (int x = 0; x < map->width; x++)
        {
            int idx = map_get_idx(map, (ivec_t){x, y});
            map->ground_layer[idx] = GRASS_LAYER_TYPE;
            map->ground_views[idx] = TEXTURE_ID_GRASS_1;
        }

    const path_node_t* path_nodes = path->path_nodes.data;

    for (int i = 0; i < path->path_nodes.size; i++)
    {
        path_node_t node = path_nodes[i];
        int current_idx = map_get_idx(map, node.pos);
        map->ground_layer[current_idx] = PATH_LAYER_TYPE;

        if (node.dirmask == MASK_DIR_LD)
            map->ground_views[current_idx] = TEXTURE_ID_PATHTURN_LD;
        else if (node.dirmask == MASK_DIR_LU)
            map->ground_views[current_idx] = TEXTURE_ID_PATHTURN_LU;
        else if (node.dirmask == MASK_DIR_RU)
            map->ground_views[current_idx] = TEXTURE_ID_PATHTURN_RU;
        else if (node.dirmask == MASK_DIR_RD)
            map->ground_views[current_idx] = TEXTURE_ID_PATHTURN_RD;
        else if (node.dirmask == MASK_DIR_TSHAPE_D)
            map->ground_views[current_idx] = TEXTURE_ID_PATHTURN_TSHAPE_D;
        else if (node.dirmask == MASK_DIR_TSHAPE_L)
            map->ground_views[current_idx] = TEXTURE_ID_PATHTURN_TSHAPE_L;
        else if (node.dirmask == MASK_DIR_TSHAPE_R)
            map->ground_views[current_idx] = TEXTURE_ID_PATHTURN_TSHAPE_R;
        else if (node.dirmask == MASK_DIR_TSHAPE_U)
            map->ground_views[current_idx] = TEXTURE_ID_PATHTURN_TSHAPE_U;
        else if (node.dirmask == MASK_DIR_CROSS)
            map->ground_views[current_idx] = TEXTURE_ID_PATHTURN_CROSS;

        if (node.dirmask & MASK_BIT_DIR_LEFT)
        {
            for (int x = node.pos.x-1; x > -1; x--)
            {
                ivec_t current_pos = {x, node.pos.y};
                int current_idx = map_get_idx(map, current_pos);
                uint16_t* layer = &(map->ground_layer[current_idx]);
                if (!bitset_get(&path->path_map, current_idx)) break;
                if (*layer == PATH_LAYER_TYPE) break;
                *layer = PATH_LAYER_TYPE;
                map->ground_views[current_idx] = TEXTURE_ID_PATHLINE_HORIZONTAL;
            }
        }
        if (node.dirmask & MASK_BIT_DIR_UP)
        {
            for (int y = node.pos.y-1; y > -1; y--)
            {
                ivec_t current_pos = {node.pos.x, y};
                int current_idx = map_get_idx(map, current_pos);
                uint16_t* layer = &(map->ground_layer[current_idx]);
                if (!bitset_get(&path->path_map, current_idx)) break;
                if (*layer == PATH_LAYER_TYPE) break;
                *layer = PATH_LAYER_TYPE;
                map->ground_views[current_idx] = TEXTURE_ID_PATHLINE_VERICAL;
            }

        }
        if (node.dirmask & MASK_BIT_DIR_RIGHT)
        {
            for (int x = node.pos.x+1; x < map->width; x++)
            {
                ivec_t current_pos = {x, node.pos.y};
                int current_idx = map_get_idx(&model->map, current_pos);
                uint16_t* layer = &(map->ground_layer[current_idx]);
                if (!bitset_get(&path->path_map, current_idx)) break;
                if (*layer == PATH_LAYER_TYPE) break;
                *layer = PATH_LAYER_TYPE;
                map->ground_views[current_idx] = TEXTURE_ID_PATHLINE_HORIZONTAL;
            }
        }
        if (node.dirmask & MASK_BIT_DIR_DOWN)
        {
            for (int y = node.pos.y+1; y < map->height; y++)
            {
                ivec_t current_pos = {node.pos.x, y};
                int current_idx = map_get_idx(&model->map, current_pos);
                uint16_t* layer = &(map->ground_layer[current_idx]);
                if (!bitset_get(&path->path_map, current_idx)) break;
                if (*layer == PATH_LAYER_TYPE) break;
                *layer = PATH_LAYER_TYPE;
                map->ground_views[current_idx] = TEXTURE_ID_PATHLINE_VERICAL;
            }
        }
    }

    /*for (int y = 0; y < map->height; y++)
    {
        for (int x = 0; x < map->width; x++)
        {
            int idx = map_get_idx(map, (ivec_t){x, y});
            int res = map->ground_layer[idx] == PATH_LAYER_TYPE;
            res += bitset_get(&path->path_map, idx);

            printf("%i", res);
        }
        printf("\n");
    }*/
}

void generate_objects_v1(model_t* model)
{
    map_t* map = &model->map;
    for (int y = 0; y < map->height; y++)
    {
        for (int x = 0; x < map->width; x++)
        {
            ivec_t current_pos = {x, y};
            int current_idx = map_get_idx(map, current_pos);
            
            if (norm_rand() > 0.5 && model->map.ground_layer[current_idx] != PATH_LAYER_TYPE)
            {
                ent_tree_t* tree = (ent_tree_t*)add_entity(model, (ivec_t) {x, y}, TREE_ENTITY);
                tree->texture_id = TEXTURE_ID_TREE_1;
            }
            else
            {
                map->entities[map_get_idx(map, (ivec_t){x, y})] = EMPTY_ENTITY;
            }
        }
    }
}

void generate_creatures_and_player_v1(model_t* model, const path_data_t* path)
{
    map_t* map = &model->map;

    int rand_id = int_rand_range(0, path->path_nodes.size);
    const path_node_t* node = dynarr_get(&path->path_nodes, rand_id);

    ent_player_t* player = (ent_player_t*)add_entity(model, node->pos, PLAYER_ENTITY);
    player->texture_id = TEXTURE_ID_PLAYER_2;
    player->health = 100;


    for (int i = 0; i < 6; i++)
    {
        int rand_id = int_rand_range(0, path->path_nodes.size);
        const path_node_t* node = dynarr_get(&path->path_nodes, rand_id);
        if (bitset_get(&map->obstacles, map_get_idx(map, node->pos)) != 0) continue;
        ent_enemy_t* enemy = (ent_enemy_t*)add_entity(model, node->pos, ENEMY_ENTITY);
        enemy->texture_id = TEXTURE_ID_ENEMY_1;
        enemy->health = 50;
    }
}

void generate_location_v2(int width, int height, model_t* model)
{
    path_data_t path = generate_path_v1(model);

    generate_ground_layer_v1(&path, model);
    generate_objects_v1(model);
    generate_creatures_and_player_v1(model, &path);

    bitset_free(&path.path_map);
    dynarr_free(&path.path_nodes);
}

void generate_location_v1(int width, int height, model_t* model)
{
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
}

void generate_location(int width, int height, model_t* model)
{   
    time_t t;
    srand((unsigned) time(&t));
    model_allocate(model, width, height);

    bitset_clear(&model->map.obstacles);

    generate_location_v2(width, height, model);

    globals_allocate_per_location();
}


void destroy_location(model_t* model)
{
    globals_free_per_location();
    model_free(model);
}

#endif // GENERATOR_H
