#ifndef RENDER_H
#define RENDER_H

#include "common.h"
#include "globals.h"
#include "model.h"
#include <raylib.h>


void render(Camera2D camera, const map_t *map, const entity_container_t* entities,
        Texture2D *textures);
void render_ground(const map_t * map, Texture2D* textures);
void render_entities(const map_t * map, const entity_container_t* entities, Texture2D* textures);
void render_effects();
void render_ingame_ui();
void render_ui();

void render(Camera2D camera, const map_t *map, const entity_container_t* entities,
        Texture2D *textures)
{
    BeginDrawing();
    ClearBackground((Color){ .r = 0, .g = 0, .b = 0, .a = 255 });
    
    BeginMode2D(camera);
    render_ground(map, textures);
    render_entities(map, entities, textures);
    render_effects();
    render_ingame_ui();
    EndMode2D();

    render_ui();
    EndDrawing();
}

void render_ground(const map_t * map, Texture2D* textures)
{
    for (int x = 0; x < map->width; x++)
    {
        for (int y = 0; y < map->height; y++)
        {
            ivec_t pos = {x, y};
            int texture_id = map->ground_views[map_get_idx(map, pos)];
            int x_pos = pos.x * TSIZE;
            int y_pos = pos.y * TSIZE;
            DrawTexture(textures[texture_id], x_pos, y_pos, WHITE);
        }
    }
}

void render_entities(const map_t * map, const entity_container_t* entities, Texture2D* textures)
{
    for (int x = 0; x < map->width; x++)
    {
        for (int y = 0; y < map->height; y++)
        {
            ivec_t pos = {x, y};
            int idx = map_get_idx(map, pos);
            entity_t entity = map->entities[idx];

            if(entity.type == UNKNOWN_ENTITY) continue;

            int x_pos = pos.x * TSIZE;
            int y_pos = pos.y * TSIZE;
            uint16_t texture_id = entity_get_texture_id(entities, entity);

            DrawTexture(textures[texture_id], x_pos, y_pos, WHITE);
        }
    }
}

void render_ui()
{
    DrawText("HERO FORESTER", 10, 10, 20, WHITE);
}

#endif // RENDER_H