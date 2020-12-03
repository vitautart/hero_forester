#ifndef MAP_H
#define MAP_H

#include <stdint.h>

typedef struct map_t
{
    uint16_t *ground_type; // grass, sand, road etc
    int width;
    int height;
}  map_t;

#endif // MAP_H
