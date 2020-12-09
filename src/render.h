#ifndef RENDER_H
#define RENDER_H

#include "globals.h"
#include "model.h"
#include <raylib.h>


void render(Camera2D camera, const map_t *map,
        Texture2D *ground_textures, Texture2D *stuff_textures);
void render_ground(const map_t * map, Texture2D* textures);
void render_entities();
void render_effects();
void render_ingame_ui();
void render_ui();

void render(Camera2D camera, const map_t *map,
        Texture2D *ground_textures, Texture2D *stuff_textures)
{
    BeginDrawing();
    ClearBackground((Color){ .r = 0, .g = 0, .b = 0, .a = 255 });
    
    BeginMode2D(camera);
    render_ground(map, ground_textures);
    render_entities();
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
            int texture_id = map->ground_views[map_get_idx(map, x, y)];
            int x_pos = x * TSIZE;
            int y_pos = y * TSIZE;
            DrawTexture(textures[texture_id], x_pos, y_pos, WHITE);
        }
    }
}

void render_ui()
{
    DrawText("HERO FORESTER", 10, 10, 20, WHITE);
}

#endif // RENDER_H
