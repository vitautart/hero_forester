#ifndef SIMULATION_H
#define SIMULATION_H

#include "globals.h"
#include "common.h"
#include "model.h"
#include "effects.h"
#include "raylib.h"
#include <stdio.h>

#define NO_ACTION_PRODUCED(input_entity) (action_t) {.type = ACTION_TYPE_NO_ACTION_PRODUCED, .entity = input_entity } 

typedef enum 
{
    ACTION_TYPE_IDLE,
    ACTION_TYPE_MOVE_TO,
    ACTION_TYPE_SMARTMOVE_TO,
    ACTION_TYPE_SHOOT,
    ACTION_TYPE_NO_ACTION_PRODUCED
} action_type_t;

typedef struct action_t
{
    action_type_t type;
    entity_t entity;
    union
    {
        float asfloat;
        int asint;
    } value;
    ivec_t map_pos_idx[2];
} action_t;

ivec_t nav_path_get_after_current(const map_t* map, dynarr_t* path)
{
    int next_map_idx = GET_AS(int, dynarr_get(&global_path, global_path.size - 2));
    return map_get_pos(map, next_map_idx);
}

action_t get_desicion_for_enemy_with_rifle(model_t* model, entity_t entity)
{
    action_t action = {.entity = entity};
    ent_enemy_t* enemy = GET_ENEMY(model, entity);
    ent_player_t* player = GET_PLAYER(model);
    ivec_t player_pos = map_get_pos(&model->map, player->mapid);
    ivec_t enemy_pos = map_get_pos(&model->map, enemy->mapid);
    int can_get_player = map_find_path(enemy_pos, player_pos, 1, &model->map, &global_path);
    if (!can_get_player)
    {
        action.type = ACTION_TYPE_IDLE;
        return action;
    }

    if (manhatten_distance(player_pos, enemy_pos) > 4)
    {   
        ivec_t next_pos = nav_path_get_after_current(&model->map, &global_path);
        action.type = ACTION_TYPE_MOVE_TO;
        action.map_pos_idx[0] = enemy_pos;
        action.map_pos_idx[1] = next_pos;
    }
    else
    {
        // TODO: this is just temporary
        // we will change this to ACTION_TYPE_SHOOT
        action.type = ACTION_TYPE_IDLE;
    }

    return action;
}

action_t produce_ai_action(model_t* model, entity_t entity)
{
    return get_desicion_for_enemy_with_rifle(model, entity);
}

entity_t next_entity_for_action(const model_t* model, entity_t current)
{
    const dynarr_t* enemies = &model->entities[ENEMY_ENTITY];
    if (current.type == PLAYER_ENTITY && enemies->size == 0)
    {
        return current;
    }
    else if (current.type == PLAYER_ENTITY && enemies->size > 0)
    {
        const ent_enemy_t* enemy = (ent_enemy_t*)dynarr_get(enemies, 0);
        return model->map.entities[enemy->mapid];
    }
    else if (current.type == ENEMY_ENTITY && current.id < (enemies->size - 1)) 
    {
        const ent_enemy_t* enemy = (ent_enemy_t*)dynarr_get(enemies, current.id + 1);
        return model->map.entities[enemy->mapid];
    }
    else if (current.type == ENEMY_ENTITY)
    {
        const ent_player_t* player = GET_PLAYER(model);
        return model->map.entities[player->mapid];
    }

    assert(0);
}

entity_t do_action(model_t* model, const action_t* action, dynarr_t* effect_emmiters)
{
    if (action->type == ACTION_TYPE_NO_ACTION_PRODUCED)
    {
        return action->entity;
    }
    else if (action->type == ACTION_TYPE_IDLE)
    {
        return next_entity_for_action(model, action->entity);
    }
    else if (action->type == ACTION_TYPE_MOVE_TO)
    {
        ivec_t current_pos = action->map_pos_idx[0]; 
        ivec_t next_pos = action->map_pos_idx[1];

        void* entity_ptr = model_get_entity_data(model, action->entity);
        ivec_t delta = ivec_sub(next_pos, current_pos); 
        uint16_t current_tex_id = entity_get_texture_id_void(entity_ptr);
        if (delta.x > 0)
        {
            uint16_t new_tex_id = global_tex_id_left_to_right_mapping[current_tex_id];
            if (new_tex_id != TEXTURE_ID_INVALID) 
                entity_set_texture_id_void(entity_ptr, new_tex_id);
        }
        else if (delta.x < 0) 
        {
            uint16_t new_tex_id = global_tex_id_right_to_left_mapping[current_tex_id];
            if (new_tex_id != TEXTURE_ID_INVALID) 
                entity_set_texture_id_void(entity_ptr, new_tex_id);
        }

        int result = move_entity(model, current_pos, next_pos);
        return result ? next_entity_for_action(model, action->entity) : action->entity;
    }
    else if (action->type == ACTION_TYPE_SMARTMOVE_TO)
    {
        int result = map_find_path(action->map_pos_idx[0], action->map_pos_idx[1], 0, &model->map, &global_path);
        if (!result) return action->entity;
        if (global_path.size < 2) return action->entity;
        
        ivec_t current_pos = action->map_pos_idx[0];

#ifdef DEBUG_RENDER
        dynarr_clear(&global_debug_grey_map_cell_pos);
        for(int i = 0; i < global_path.size; i++)
        {
            int debug_pos_id = GET_AS(int, dynarr_get(&global_path, i));
            ivec_t debug_pos = map_get_pos(&model->map, debug_pos_id);
            dynarr_add(&global_debug_grey_map_cell_pos, &debug_pos);
        }
#endif

        ivec_t next_pos = nav_path_get_after_current(&model->map, &global_path);
        result = move_entity(model, current_pos, next_pos);
        
        if (result && (action->entity.type == PLAYER_ENTITY))
        {
            ent_player_t* player = GET_PLAYER(model);
            ivec_t delta = ivec_sub(next_pos, current_pos); 
            if (delta.x > 0) player->texture_id = TEXTURE_ID_PLAYER_1;
            else if (delta.x < 0) player->texture_id = TEXTURE_ID_PLAYER_2;
        }

        return result ? next_entity_for_action(model, action->entity) : action->entity;
    }
    else if (action->type == ACTION_TYPE_SHOOT)
    {
        ivec_t start = action->map_pos_idx[0];
        ivec_t end = action->map_pos_idx[1];
        int end_idx = map_get_idx(&model->map, end);
        entity_t end_entity = model->map.entities[end_idx];

        if (end_entity.type == UNKNOWN_ENTITY) return action->entity;
        if (!(model->components[end_entity.type] & COMPONENT_BIT_FLAG_HEALTH)) return action->entity;

        bresenham(start, end, &global_bresenham_line);

        ivec_t current_pos;
        for (int i = 1; i < global_bresenham_line.size - 1; i++)
        {
            current_pos = GET_AS(ivec_t, dynarr_get(&global_bresenham_line, i));
            int map_id = map_get_idx(&model->map, current_pos);
            if (bitset_get(&model->map.obstacles, map_id))
            {
                return action->entity;
            }
        }

        void* end_entity_ptr = model_get_entity_data(model, end_entity);
        int current_health = entity_get_health_void(end_entity_ptr);
        current_health -= action->value.asint;

        if (current_health <= 0)
        {
            remove_entity(model, end);
        }
        else
        {
            effect_emmiter_t emmiter = 
            {
                .type = EFFECT_TYPE_SHOOT,
                .pos = {start, end}
            };
            dynarr_add(effect_emmiters, &emmiter);
            entity_set_health_void(end_entity_ptr, current_health);
        }

        return next_entity_for_action(model, action->entity);

    }
    assert(0);
}

#endif // SIMULATION_H
