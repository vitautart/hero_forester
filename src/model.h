#ifndef MODEL_H
#define MODEL_H

#include "globals.h"

#include <stddef.h>
#include <stdint.h>
#include <raylib.h>
#include <raymath.h>

/*typedef enum
{
    AUTO_MOVE_SHOOT_USER_MODE,
    MOVE_USER_MODE,
    SHOOT_USER_MODE,
    OPEN_INVENTORY_USER_MODE,
    WAIT_FOR_OTHERS_USER_MODE
} user_command_mode_t;

typedef struct user_state_t
{
    user_command_mode_t command_mode;
} user_state_t;*/

typedef enum
{
    PLAYER_ENTITY,
    ENEMY_ENTITY,
    TREE_ENTITY,
    ENTITY_TYPE_LENGTH, // allways keep this to the end of list of valid enums 
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
#define GET_ENEMY(model, entity) (ent_enemy_t*)dynarr_get(&model->entities[entity.type], entity.id)

typedef enum 
{
    COMPONENT_BIT_FLAG_HEALTH = 1,
} component_bit_flag_t;

typedef enum
{
    COMPONENT_SET_PLAYER = 0 | COMPONENT_BIT_FLAG_HEALTH,
    COMPONENT_SET_ENEMY = 0 | COMPONENT_BIT_FLAG_HEALTH,
    COMPONENT_SET_TREE = 0,
} component_set_t;

typedef struct entity_t
{
    entity_type_t type;
    int id;
} entity_t;

typedef struct ent_player_t
{
    int mapid;
    uint16_t texture_id; // not move this position of this field
    entity_type_t type;
    int health;
} ent_player_t;

typedef struct ent_enemy_t
{
    int mapid;
    uint16_t texture_id; // not move this position of this field
    entity_type_t type;
    int health;
} ent_enemy_t;

typedef struct ent_tree_t
{
    int mapid;
    uint16_t texture_id; // not move this position of this field
    entity_type_t type;
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
    dynarr_t entities[ENTITY_TYPE_LENGTH];
    component_set_t components[ENTITY_TYPE_LENGTH];
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

static inline int map_check_pos_outside(const map_t* map, ivec_t pos)
{
    return pos.x < 0 || pos.x >= map->width || pos.y < 0 || pos.y >= map->height;
}

iaabb_t map_get_size_as_box(const map_t* map)
{
    iaabb_t result;
    return (iaabb_t)
    {
        .min = {0, 0},
        .max = {map->width-1, map->height-1}
    };
}

void map_free (map_t * map)
{
    free(map->ground_layer);
    free(map->ground_views);
    free(map->entities);
    bitset_free(&map->obstacles);
}

void model_allocate(model_t* model, int width, int height)
{
    int enemies_expect_count = 32;
    int tree_expect_count = width * height;

    map_allocate(&model->map, width, height);

    model->components[PLAYER_ENTITY] = COMPONENT_SET_PLAYER;
    model->components[ENEMY_ENTITY] = COMPONENT_SET_ENEMY;
    model->components[TREE_ENTITY] = COMPONENT_SET_TREE;

    model->entities[PLAYER_ENTITY] = dynarr_allocate(sizeof(ent_player_t), 0, 1);
    model->entities[ENEMY_ENTITY] = dynarr_allocate(sizeof(ent_enemy_t), 0, enemies_expect_count);
    model->entities[TREE_ENTITY] = dynarr_allocate(sizeof(ent_enemy_t), 0, tree_expect_count);
}

void model_free(model_t* model)
{
    for (int i = 0; i < ENTITY_TYPE_LENGTH; i++)
        dynarr_free(&model->entities[i]);
}

#define CREATE_GETTER_FOR_MEMBER(type, member, ref_type) static inline type entity_get_##member##_void (void* entity_ptr) { return *(type*)(entity_ptr + offsetof(ref_type, member)); }

#define CREATE_SETTER_FOR_MEMBER(type, member, ref_type) static inline void entity_set_##member##_void (void* entity_ptr, type member){ type* ptr = (type*)(entity_ptr + offsetof(ref_type, member)); *ptr = member; }

/****** MAPID GETTERS SETTERS ******/

CREATE_GETTER_FOR_MEMBER(int, mapid, ent_player_t)    
CREATE_SETTER_FOR_MEMBER(int, mapid, ent_player_t)

int entity_get_mapid (const model_t* model, entity_t entity)
{
    return entity_get_mapid_void(dynarr_get(&model->entities[entity.type], entity.id));
}

void entity_set_mapid (model_t* model, entity_t entity, int mapid)
{
    entity_set_mapid_void(dynarr_get(&model->entities[entity.type], entity.id), mapid);
}

/****** TEXTURE_ID GETTERS SETTERS ******/

CREATE_GETTER_FOR_MEMBER(uint16_t, texture_id, ent_player_t)    
CREATE_SETTER_FOR_MEMBER(uint16_t, texture_id, ent_player_t)

uint16_t entity_get_texture_id (const model_t* model, entity_t entity)
{
    return entity_get_texture_id_void(dynarr_get(&model->entities[entity.type], entity.id));
}

/****** TYPE GETTERS SETTERS ******/

CREATE_GETTER_FOR_MEMBER(entity_type_t, type, ent_player_t)    
CREATE_SETTER_FOR_MEMBER(entity_type_t, type, ent_player_t)

entity_type_t entity_get_type (const model_t* model, entity_t entity)
{
    return entity_get_type_void(dynarr_get(&model->entities[entity.type], entity.id));
}

void entity_set_type (model_t* model, entity_t entity, entity_type_t type)
{
    entity_set_type_void(dynarr_get(&model->entities[entity.type], entity.id), type);
}

/****** HEALTH GETTERS SETTERS ******/


CREATE_GETTER_FOR_MEMBER(int, health, ent_player_t)
CREATE_SETTER_FOR_MEMBER(int, health, ent_player_t)

int entity_get_health (const model_t* model, entity_t entity)
{
    return GET_AS(int, dynarr_get(&model->entities[entity.type], entity.id));
}

void entity_set_health (model_t* model, entity_t entity, int health)
{
    int* ptr = (int*)dynarr_get(&model->entities[entity.type], entity.id);
    *ptr = health;
}

/*******************/
void* add_entity (model_t* model, ivec_t pos, entity_type_t type)
{
    int idx = map_get_idx(&model->map, pos);
    dynarr_t *arr = &model->entities[type];
    assert(dynarr_increment(arr) != COM_ERR);

    void* entity_ptr = dynarr_get_last(arr);

    entity_t* e = &model->map.entities[idx];
    e->type = type;
    e->id = arr->size - 1;
    //entity_set_mapid(model, *e, idx);
    entity_set_mapid_void(entity_ptr, idx);
    entity_set_type_void(entity_ptr, type);

    bitset_set(&model->map.obstacles, idx, 1);

    return dynarr_get(arr, e->id);
}

void remove_entity(model_t* model, ivec_t pos)
{
    int idx = map_get_idx(&model->map, pos);
    entity_t entity = model->map.entities[idx];
    model->map.entities[idx] = EMPTY_ENTITY;
    bitset_set(&model->map.obstacles, idx, 0);

    dynarr_t *arr = &model->entities[entity.type];
    if (dynarr_remove_swap(arr, entity.id)) // if swap was present than TRUE
    {
        int mapid = entity_get_mapid(model, entity);
        model->map.entities[mapid].id = entity.id;
    }
}

static inline entity_t model_get_entity(const model_t* model, void* entity_data)
{
    int map_id = entity_get_mapid_void(entity_data);
    return model->map.entities[map_id];
}

static inline void* model_get_entity_data(const model_t* model, entity_t entity)
{
    return dynarr_get(&model->entities[entity.type], entity.id);
}

static inline void* model_get_entity_data_by_pos(const model_t* model, ivec_t pos)
{
    int idx = map_get_idx(&model->map, pos);
    entity_t e = model->map.entities[idx];
    return model_get_entity_data(model, e); 
}

// return 0 - not moved
// return 1 - moved
int move_entity(model_t* model, ivec_t from, ivec_t to)
{
    if (map_check_pos_outside(&model->map, to)) return 0;
    
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

static inline int manhatten_distance(ivec_t p1, ivec_t p2)
{
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}


void reconstruct_path(int start_idx, int end_idx, dynarr_t* path)
{
    dynarr_add(path, &end_idx);
    int current = end_idx;
    int neighbore;
    while (current != start_idx)
    {
        assert(hashmap_get(&global_path_links, current, &neighbore));
        current = neighbore;
        dynarr_add(path, &current);
    }
}

int map_find_path(ivec_t start, ivec_t end, int radius, const map_t* map, dynarr_t* path)
{
    minheap_clear(&global_open_set);
    hashmap_clear(&global_path_links);
    hashmap_clear(&global_g_score);
    dynarr_clear(path);

    int end_idx = map_get_idx(map, end);
    int start_idx = map_get_idx(map, start);

    minheap_push(&global_open_set, (qnode_t){ manhatten_distance(start, start), start_idx});
    hashmap_add_or_replace(&global_g_score, (qnode_t){start_idx, 0});

    qnode_t current;
    while(minheap_pop(&global_open_set, &current))
    {
        int current_idx = current.value;
        ivec_t current_pos = map_get_pos(map, current_idx);

        if (manhatten_distance(current_pos, end) <= radius) 
        {
            reconstruct_path(start_idx, current_idx, path);
            return 1;
        }

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
            if (map_check_pos_outside(map, ngbr_pos)) continue;

            int ngbr_idx = map_get_idx(map, ngbr_pos);

            if (bitset_get(&map->obstacles, ngbr_idx)) continue;

            int g_score_current_value;
            assert(hashmap_get(&global_g_score, current_idx, &g_score_current_value));
            int tentative_score = g_score_current_value + 1;

            int g_score_ngbr_value;
            int g_score_result = hashmap_get(&global_g_score, ngbr_idx, &g_score_ngbr_value);
            if (g_score_result == 0 || tentative_score < g_score_ngbr_value)
            {
                hashmap_add_or_replace(&global_path_links, (qnode_t){ngbr_idx, current_idx});
                hashmap_add_or_replace(&global_g_score, (qnode_t){ngbr_idx, tentative_score});

                int f_score = tentative_score + manhatten_distance(end, ngbr_pos);

                // TODO: RECONSIDER THIS
                // SEE FIRST TODO IN THIS FUNCTION
                minheap_push(&global_open_set, (qnode_t){f_score, ngbr_idx});
            }
        }
    }
    return 0;
}

void bresenham_low(ivec_t v0, ivec_t v1, dynarr_t* line)
{
    ivec_t delta = ivec_sub(v1, v0);
    int inc_y = 1;
    if(delta.y < 0)
    {
        inc_y = -1;
        delta.y = -delta.y;
    }
    int diff = 2 * delta.y - delta.x;

    int current_y = v0.y;

    int delta_y_minus_x = delta.y - delta.x;
    for(int current_x = v0.x; current_x <= v1.x; current_x++)
    {
        ivec_t current_pos = {current_x, current_y};
        dynarr_add(line, &current_pos);
        if (diff > 0)
        {
            current_y += inc_y;
            diff += 2 * delta_y_minus_x;
        }
        else
        {
            diff += 2 * delta.y;
        }
    }
}

void bresenham_high(ivec_t v0, ivec_t v1, dynarr_t* line)
{
    ivec_t delta = ivec_sub(v1, v0);
    int inc_x = 1;
    if(delta.x < 0)
    {
        inc_x = -1;
        delta.x = -delta.x;
    }
    int diff = 2 * delta.x - delta.y;

    int current_x = v0.x;

    int delta_x_minus_y = delta.x - delta.y;
    for(int current_y = v0.y; current_y <= v1.y; current_y++)
    {
        ivec_t current_pos = {current_x, current_y};
        dynarr_add(line, &current_pos);
        if (diff > 0)
        {
            current_x += inc_x;
            diff += 2 * delta_x_minus_y;
        }
        else
        {
            diff += 2 * delta.x;
        }
    }
}

void bresenham(ivec_t v0, ivec_t v1, dynarr_t* line)
{
    dynarr_clear(line);

    if (abs(v1.y - v0.y) < abs(v1.x - v0.x))
    {
        if (v0.x > v1.x) bresenham_low(v1, v0, line);
        else bresenham_low(v0, v1, line);
    }
    else
    {
        if (v0.y > v1.y) bresenham_high(v1, v0, line);
        else bresenham_high(v0, v1, line);
    }
}

// returns last element
/*ivec_t map_raycast(const map_t* map, ivec_t start, ivec_t end)
{

}*/

#endif // MODEL_H
