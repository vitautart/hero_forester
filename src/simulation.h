#ifndef SIMULATION_H
#define SIMULATION_H

#include "globals.h"
#include "common.h"
#include "model.h"
#include "presentation.h" // TODO: something wrong with this dependencies
#include "raylib.h"
#include <stdio.h>

#define NO_ACTION_PRODUCED(input_entity) (action_t) {.type = ACTION_TYPE_NO_ACTION_PRODUCED, .entity = input_entity } 

typedef enum 
{
    ACTION_TYPE_IDLE,
    ACTION_TYPE_MOVE_TO,
    ACTION_TYPE_SMARTMOVE_TO,
    ACTION_TYPE_NO_ACTION_PRODUCED
} action_type_t;

typedef struct action_t
{
    action_type_t type;
    entity_t entity;
    ivec_t map_pos_idx[2];
} action_t;

static minheap_t global_open_set;
static hashmap_t global_path_links;
static hashmap_t global_g_score;
static bitset_t global_open_set_pops_tracker;
static dynarr_t global_path;
static int global_current_id;

action_t produce_user_action(model_t* model, entity_t entity, user_state_t* user_state, Camera2D camera)
{
    ent_player_t* player = GET_PLAYER(model);

    if (user_state->command_mode == AUTO_MOVE_SHOOT_USER_MODE 
        || user_state->command_mode == MOVE_USER_MODE)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            ivec_t mouse_pos = map_get_mouse_pos(camera);
            if (map_check_pos_outside(&model->map, mouse_pos)) return NO_ACTION_PRODUCED(entity);
            ivec_t current = map_get_pos(&model->map, player->mapid);
            return (action_t) {.type = ACTION_TYPE_SMARTMOVE_TO, .entity = entity, .map_pos_idx = {current, mouse_pos}};
        }
        else if (IsKeyPressed(KEY_LEFT))
        {
            ivec_t current = map_get_pos(&model->map, player->mapid);
            ivec_t next = ivec_add(current, (ivec_t){-1, 0});
            return (action_t) {.type = ACTION_TYPE_MOVE_TO, .entity = entity, .map_pos_idx = {current, next}};
        }
        else if (IsKeyPressed(KEY_RIGHT))
        {
            ivec_t current = map_get_pos(&model->map, player->mapid);
            ivec_t next = ivec_add(current, (ivec_t){1, 0});
            return (action_t) {.type = ACTION_TYPE_MOVE_TO, .entity = entity, .map_pos_idx = {current, next}};
        }
        else if (IsKeyPressed(KEY_UP))
        {
            ivec_t current = map_get_pos(&model->map, player->mapid);
            ivec_t next = ivec_add(current, (ivec_t){0, -1});
            return (action_t) {.type = ACTION_TYPE_MOVE_TO, .entity = entity, .map_pos_idx = {current, next}};
        }
        else if (IsKeyPressed(KEY_DOWN))    
        {
            ivec_t current = map_get_pos(&model->map, player->mapid);
            ivec_t next = ivec_add(current, (ivec_t){0, 1});
            return (action_t) {.type = ACTION_TYPE_MOVE_TO, .entity = entity, .map_pos_idx = {current, next}};
        }
    }

    return NO_ACTION_PRODUCED(entity);
}

action_t produce_ai_action(model_t* model, entity_t entity)
{
    return (action_t) 
    {
        .type = ACTION_TYPE_IDLE,
        .entity = entity
    };
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

    assert(false);
}

entity_t do_action(model_t* model, const action_t* action)
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
        if (action->entity.type == PLAYER_ENTITY)
        {
            ent_player_t* player = GET_PLAYER(model);
            ivec_t delta = ivec_sub(next_pos, current_pos); 
            if (delta.x > 0) player->texture_id = TEXTURE_ID_PLAYER_1;
            else if (delta.x < 0) player->texture_id = TEXTURE_ID_PLAYER_2;
        }
        int result = move_entity(model, current_pos, next_pos);
        return result ? next_entity_for_action(model, action->entity) : action->entity;
    }
    else if (action->type == ACTION_TYPE_SMARTMOVE_TO)
    {
        int result = map_find_path(action->map_pos_idx[0], action->map_pos_idx[1], &model->map, &global_open_set, &global_path_links, &global_g_score, &global_open_set_pops_tracker, &global_path);
        if (!result) return action->entity;
        if (global_path.size < 2) return action->entity;
        int next_map_idx = GET_AS(int, dynarr_get(&global_path, global_path.size - 2));

        ivec_t current_pos = action->map_pos_idx[0]; 
        ivec_t next_pos = map_get_pos(&model->map, next_map_idx);
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
    assert(false);
}

#endif // SIMULATION_H
