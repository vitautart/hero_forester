#ifndef PRESENTATION_H
#define PRESENTATION_H

#include "globals.h"
#include "common.h"

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

#endif // PRESENTATION_H
