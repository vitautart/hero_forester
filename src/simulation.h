#ifndef SIMULATION_H
#define SIMULATION_H

#include "globals.h"
#include "common.h"
#include "model.h"
#include "presentation.h" // TODO: something wrong with this dependencies
#include "raylib.h"
#include <stdio.h>

typedef enum
{
    ROUND_WAIT, // WAITING FOR PLAYER INPUT
    ROUND_PLAYER, // PLAYER DID TURN
    ROUND_ENEMY, // ENEMY DID TURN
} round_t;

static minheap_t global_open_set;
static hashmap_t global_path_links;
static hashmap_t global_g_score;
static bitset_t global_open_set_pops_tracker;
static dynarr_t global_path;

round_t simulate(model_t* model, user_state_t* user_state, Camera2D camera, round_t round);
int player_turn(model_t* model, user_state_t* user_state, Camera2D camera);
int enemies_turn(model_t* model);
void do_turn(ent_enemy_t* enemy, map_t* map);

round_t simulate(model_t* model, user_state_t* user_state, Camera2D camera, round_t round)
{
    // TODO:    hardcoded default value for player active mode MOVE_USER_MODE is not OK.
    //          we must change this to some active tool in user ui that corresponds to certain user mode.
    user_state->command_mode = round != ROUND_WAIT ? WAIT_FOR_OTHERS_USER_MODE : MOVE_USER_MODE;

    if (round == ROUND_WAIT)
    {
        return player_turn(model, user_state, camera) ? ROUND_PLAYER : ROUND_WAIT;
    }
    else if (round == ROUND_PLAYER || round == ROUND_ENEMY)
    {
        return enemies_turn(model) ? ROUND_WAIT : ROUND_ENEMY;
    }

    return ROUND_WAIT;
}

int player_turn(model_t* model, user_state_t* user_state, Camera2D camera)
{
    ent_player_t* player = GET_PLAYER(model);

    if (user_state->command_mode == AUTO_MOVE_SHOOT_USER_MODE 
        || user_state->command_mode == MOVE_USER_MODE)
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            // TODO: add check for obstacle mouse position
            ivec_t mouse_pos = map_get_mouse_pos(camera);
            printf("cursor pressed: %i %i\n", mouse_pos.x, mouse_pos.y);
            if (map_check_pos_outside(&model->map, mouse_pos)) return 0;
            printf("cursor inside map\n");
            ivec_t current = map_get_pos(&model->map, player->mapid);
            int result = map_find_path(current, mouse_pos, &model->map, &global_open_set, &global_path_links, &global_g_score, &global_open_set_pops_tracker, &global_path);

            printf("result is: %i\n", result);
            if (!result) return 0;

            printf("global_path length: %i\n", global_path.size);
            int next_map_idx = GET_AS(int, dynarr_get(&global_path, global_path.size - 2));
            printf("next_map_idx: %i\n", next_map_idx);
            return move_entity(model, current, map_get_pos(&model->map, next_map_idx));
        }
        if (IsKeyPressed(KEY_LEFT))
        {
            player->texture_id = TEXTURE_ID_PLAYER_2;
            ivec_t current = map_get_pos(&model->map, player->mapid);
            ivec_t next = ivec_add(current, (ivec_t){-1, 0});
            return move_entity(model, current, next);
        }
        else if (IsKeyPressed(KEY_RIGHT))
        {
            player->texture_id = TEXTURE_ID_PLAYER_1;
            ivec_t current = map_get_pos(&model->map, player->mapid);
            ivec_t next = ivec_add(current, (ivec_t){1, 0});
            return move_entity(model, current, next);   
        }
        else if (IsKeyPressed(KEY_UP))
        {
            ivec_t current = map_get_pos(&model->map, player->mapid);
            ivec_t next = ivec_add(current, (ivec_t){0, -1});
            return move_entity(model, current, next);   
        }
        else if (IsKeyPressed(KEY_DOWN))    
        {
            ivec_t current = map_get_pos(&model->map, player->mapid);
            ivec_t next = ivec_add(current, (ivec_t){0, 1});
            return move_entity(model, current, next);   
        }
    }

    return 0;
}

// TODO; add more sophisticated logic
// TODO: separate turns of enemies to different cycles of game
int enemies_turn(model_t* model)
{
    for (int i = 0; i < model->entities[ENEMY_ENTITY].size; i++)
    {
        ent_enemy_t* enemy = dynarr_get(&model->entities[ENEMY_ENTITY], i);
        do_turn(enemy, &model->map);
    }

    return 1;
}

void do_turn(ent_enemy_t* enemy, map_t* map)
{
}

#endif // SIMULATION_H
