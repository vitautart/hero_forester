#ifndef SIMULATION_H
#define SIMULATION_H

#include "globals.h"
#include "common.h"
#include "model.h"

typedef enum
{
    ROUND_WAIT,
    ROUND_PLAYER,
    ROUND_ENEMY,
} round_t;

round_t simulate(model_t* model, round_t round);
int player_turn(model_t* model);
int enemies_turn(model_t* model);
void do_turn(ent_enemy_t* enemy, map_t* map);

round_t simulate(model_t* model, round_t round)
{
    if (round == ROUND_WAIT)
    {
        return player_turn(model) ? ROUND_PLAYER : ROUND_WAIT;
    }
    else if (round == ROUND_PLAYER || round == ROUND_ENEMY)
    {
        return enemies_turn(model) ? ROUND_WAIT : ROUND_ENEMY;
    }

    return ROUND_WAIT;
}

int player_turn(model_t* model)
{
    ent_player_t* player = GET_PLAYER(model);
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
