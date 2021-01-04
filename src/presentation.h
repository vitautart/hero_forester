#ifndef PRESENTATION_H
#define PRESENTATION_H

#include "globals.h"
#include "common.h"
#include "effects.h"

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
    effect_t effect = 
    {
        .pos = emmiter->pos[1],
        .lifetime = 8,
        .texture_id = TEXTURE_ID_EFFECT_1 
    };

    dynarr_add(effects, &effect);
}

void mutate_data_for_renderer (dynarr_t* effects)
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

void convert_data_for_renderer(dynarr_t* effect_emmiters, dynarr_t* effects)
{
    if (effect_emmiters->size == 0) return;

    effect_emmiter_t* emmiters_ptr = dynarr_get(effect_emmiters, 0);

    for (int i = 0; i < effect_emmiters->size; i++)
    {
        switch(emmiters_ptr->type)
        {
            case EFFECT_TYPE_SHOOT:
            {
                emmiter_to_effects_type_shoot(emmiters_ptr, effects);
                break;
            }
        }

        emmiters_ptr++;
    }

    dynarr_clear(effect_emmiters);
}

#endif // PRESENTATION_H
