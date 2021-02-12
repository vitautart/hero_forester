#ifndef USERACTIONS_H
#define USERACTIONS_H

#include "common.h"
#include "simulation.h"
#include "presentation.h"

action_t produce_user_action(model_t* model, entity_t entity, Camera2D camera, int mouse_over_map)
{
    ent_player_t* player = GET_PLAYER(model);

    if (mouse_over_map)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            ivec_t mouse_pos = map_get_mouse_pos(camera);
            if (map_check_pos_outside(&model->map, mouse_pos)) return NO_ACTION_PRODUCED(entity);
            ivec_t current = map_get_pos(&model->map, player->mapid);
            return (action_t) {.type = ACTION_TYPE_SMARTMOVE_TO, .entity = entity, .map_pos_idx = {current, mouse_pos}};
        }
        else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            ivec_t mouse_pos = map_get_mouse_pos(camera);
            if (map_check_pos_outside(&model->map, mouse_pos)) return NO_ACTION_PRODUCED(entity);
            ivec_t current = map_get_pos(&model->map, player->mapid);
            return (action_t) {
                .type = ACTION_TYPE_SHOOT,
                .entity = entity,
                .map_pos_idx = {current, mouse_pos},
                .value = { .asint =  20 }
            };
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

#endif // USERACTIONS_H
