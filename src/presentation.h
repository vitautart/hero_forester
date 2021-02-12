#ifndef PRESENTATION_H
#define PRESENTATION_H

#include "globals.h"
#include "common.h"
#include "effects.h"
#include "model.h"

#include <raylib.h>

static inline ivec_t world_to_map(Vector2 world_pos)
{
    return (ivec_t)
    {
        .x = (int)world_pos.x / TSIZE - (world_pos.x < 0 ? 1 : 0),
        .y = (int)world_pos.y / TSIZE - (world_pos.y < 0 ? 1 : 0) 
    };
}

// transforms world coordinate in camera space to map discrete coordinate
// base point in tile - left upper corner
static inline Vector2 map_to_world_lu_offset(ivec_t map_pos)
{
    return (Vector2)
    {
        .x = map_pos.x * TSIZE,
        .y = map_pos.y * TSIZE 
    };
}

// transforms world coordinate in camera space to map discrete coordinate
// base point in tile - center of tile
static inline Vector2 map_to_world_c_offset(ivec_t map_pos)
{
    return (Vector2)
    {
        .x = map_pos.x * TSIZE + (float)TSIZE * 0.5f,
        .y = map_pos.y * TSIZE + (float)TSIZE * 0.5f
    };
}

ivec_t map_get_mouse_pos(Camera2D camera)
{
    return world_to_map(GetScreenToWorld2D(GetMousePosition(), camera));
}

void emmiter_to_effects_type_shoot(const effect_emmiter_t* emmiter, dynarr_t* effects)
{
    effect_t effect_splash = 
    {
        .type = EFFECT_TYPE_SPLASH,
        .lifetime = 8,
        .splash = 
        {
            .pos = emmiter->pos[1],
            .texture_id = TEXTURE_ID_EFFECT_1 
        }
    };

    effect_t effect_proj = 
    {
        .type = EFFECT_TYPE_PROJECTILE,
        .lifetime = 8,
        .projectile = 
        {
            .start = emmiter->pos[0],
            .end = emmiter->pos[1]
        }
    };

    dynarr_add(effects, &effect_splash);
    dynarr_add(effects, &effect_proj);
}

void present_mutate_for_renderer (dynarr_t* effects)
{
    if (effects->size == 0) return;

    effect_t* effect = dynarr_get(effects, 0);

    for (int i = 0; i < effects->size; i++)
    {
        effect->lifetime--;
        effect++;
    }

    effect = dynarr_get(effects, 0);
    int effect_id = 0;
    while (effect_id < effects->size)
    {
        if (effect->lifetime <= 0) 
        {
            dynarr_remove_swap(effects, effect_id);
        }
        else
        {
            effect++;
            effect_id++;
        }
    }
}

void present_convert_for_renderer(dynarr_t* effect_emmiters, dynarr_t* effects)
{
    if (effect_emmiters->size == 0) return;

    effect_emmiter_t* emmiters_ptr = dynarr_get(effect_emmiters, 0);

    for (int i = 0; i < effect_emmiters->size; i++)
    {
        switch(emmiters_ptr->type)
        {
            case EMITTER_TYPE_SHOOT:
            {
                emmiter_to_effects_type_shoot(emmiters_ptr, effects);
                break;
            }
        }

        emmiters_ptr++;
    }

    dynarr_clear(effect_emmiters);
}

void present_sync_camera(Camera2D* camera, const model_t* model, int screen_w, int screen_h)
{
    ent_player_t* player = GET_PLAYER(model);

    ivec_t pos = map_get_pos(&model->map, player->mapid);

    Vector2 left_up_world = GetScreenToWorld2D((Vector2){0,0}, *camera);
    Vector2 right_bottom_world = GetScreenToWorld2D((Vector2){screen_w,screen_h}, *camera);
    Vector2 world_camera_delta = Vector2Subtract(right_bottom_world, left_up_world);
    Vector2 world_camera_delta_half = Vector2Scale(world_camera_delta, 0.5f); 

    camera->target = map_to_world_c_offset(pos);
    camera->offset = Vector2Scale(world_camera_delta_half, camera->zoom);
}

#endif // PRESENTATION_H
