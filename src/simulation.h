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

#define GET_PLAYER(container) (ent_player_t*)dynarr_get(&container->entities[PLAYER_ENTITY], 0)

round_t simulate(entity_container_t* container, map_t* map, round_t round);
int player_turn(entity_container_t* container, map_t* map);
int enemies_turn(entity_container_t* container, map_t* map);
void do_turn(ent_enemy_t* enemy, map_t* map);

round_t simulate(entity_container_t* container, map_t* map, round_t round)
{
    if (round == ROUND_WAIT)
    {
        return player_turn(container, map) ? ROUND_PLAYER : ROUND_WAIT;
    }
    else if (round == ROUND_PLAYER || round == ROUND_ENEMY)
    {
        return enemies_turn(container, map) ? ROUND_WAIT : ROUND_ENEMY;
    }

    return ROUND_WAIT;
}

int player_turn(entity_container_t* container, map_t* map)
{
    ent_player_t* player = GET_PLAYER(container);
    if (IsKeyPressed(KEY_LEFT))
    {
        player->texture_id = TEXTURE_ID_PLAYER_2;
        ivec_t current = map_get_pos(map, player->mapid);
        ivec_t next = ivec_add(current, (ivec_t){-1, 0});
        return move_entity(container, map, current, next);
    }
    else if (IsKeyPressed(KEY_RIGHT))
    {
        player->texture_id = TEXTURE_ID_PLAYER_1;
        ivec_t current = map_get_pos(map, player->mapid);
        ivec_t next = ivec_add(current, (ivec_t){1, 0});
        return move_entity(container, map, current, next);   
    }
    else if (IsKeyPressed(KEY_UP))
    {
        ivec_t current = map_get_pos(map, player->mapid);
        ivec_t next = ivec_add(current, (ivec_t){0, -1});
        return move_entity(container, map, current, next);   
    }
    else if (IsKeyPressed(KEY_DOWN))    
    {
        ivec_t current = map_get_pos(map, player->mapid);
        ivec_t next = ivec_add(current, (ivec_t){0, 1});
        return move_entity(container, map, current, next);   
    }
 

    return 0;
}

// TODO; add more sophisticated logic
// TODO: separate turns of enemies to different cycles of game
int enemies_turn(entity_container_t* container, map_t* map)
{
    //if (container->entities[ENEMY_ENTITY].size == 0) return 1;
    for (int i = 0; i < container->entities[ENEMY_ENTITY].size; i++)
    {
        ent_enemy_t* enemy = dynarr_get(&container->entities[ENEMY_ENTITY], i);
        do_turn(enemy, map);
    }

    return 1;
}

void do_turn(ent_enemy_t* enemy, map_t* map)
{
}

#endif // SIMULATION_H
