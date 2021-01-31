#ifndef EFFECTS_H
#define EFFECTS_H

#include "common.h"
#include "globals.h"
#include <stdint.h>

typedef enum
{
    EMITTER_TYPE_SHOOT,
} emitter_type_t;

typedef enum
{
    EFFECT_TYPE_SPLASH,
    EFFECT_TYPE_PROJECTILE
} effect_type_t;

typedef struct effect_emmiter_t
{
    emitter_type_t type;
    ivec_t pos[3];
} effect_emmiter_t;

typedef struct effect_t
{
    effect_type_t type;
    int lifetime;
    union
    {
        struct // EFFECT_TYPE_SPLASH
        {
            ivec_t pos;
            uint16_t texture_id;
        } splash;
        struct // EFFECT_TYPE_PROJECTILE
        {
            ivec_t start;
            ivec_t end;
        } projectile;
    };

} effect_t;

#endif // EFFECTS_H
