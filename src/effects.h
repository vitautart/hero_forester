#ifndef EFFECTS_H
#define EFFECTS_H

#include "common.h"
#include "globals.h"
#include <stdint.h>

typedef enum
{
    EFFECT_TYPE_SHOOT,
} effect_type_t;

typedef struct effect_emmiter_t
{
    effect_type_t type;
    ivec_t pos[3];
} effect_emmiter_t;

typedef struct effect_t
{
    ivec_t pos;
    uint16_t texture_id;
    int lifetime;
} effect_t;

#endif // EFFECTS_H
