#ifndef RENDER_H
#define RENDER_H

#include "common.h"
#include "presentation.h"
#include "model.h"

iaabb_t camera_get_map_boundaries(Camera2D camera, int screen_w, int screen_h);
iaabb_t calculate_draw_bounds(Camera2D camera, const map_t* map, int screen_w, int screen_h, int safety_extent);
void render(Camera2D camera, const model_t* map, const Texture2D *textures, int screen_w, int screen_h);
void render_ground(const map_t * map, const Texture2D* textures, const iaabb_t* bounds);
void render_entities(const model_t* model, const Texture2D* textures, const iaabb_t* bounds);
void render_effects();
void render_ingame_ui();
void render_ui();

iaabb_t camera_get_map_boundaries(Camera2D camera, int screen_w, int screen_h)
{
    Vector2 left_up_world = GetScreenToWorld2D((Vector2){0,0}, camera);
    Vector2 right_bottom_world = GetScreenToWorld2D((Vector2){screen_w,screen_h}, camera);
    return (iaabb_t) 
    {
        .min = world_to_map(left_up_world),
        .max = world_to_map(right_bottom_world)
    };
}

iaabb_t calculate_draw_bounds(Camera2D camera, const map_t* map, int screen_w, int screen_h, int safety_extent)
{
    iaabb_t bounds = camera_get_map_boundaries(camera, screen_w, screen_h);
    bounds.min = ivec_add(bounds.min, (ivec_t){-safety_extent,-safety_extent});
    bounds.max = ivec_add(bounds.max, (ivec_t){ safety_extent, safety_extent});
    iaabb_t map_box = map_get_size_as_box(map);
    bounds.min = ivec_clamp(bounds.min, map_box.min, map_box.max);
    bounds.max = ivec_clamp(bounds.max, map_box.min, map_box.max);
    return bounds;
}

void render(Camera2D camera, const model_t* model, const Texture2D *textures, int screen_w, int screen_h)
{
    iaabb_t bounds = calculate_draw_bounds(camera, &model->map, screen_w, screen_h, 1);
    BeginDrawing();
    ClearBackground((Color){ .r = 0, .g = 0, .b = 0, .a = 255 });
    
    BeginMode2D(camera);
    render_ground(&model->map, textures, &bounds);
    render_entities(model, textures, &bounds);
    render_effects();
    render_ingame_ui();
    EndMode2D();

    render_ui();
    EndDrawing();
}

void render_ground(const map_t * map, const Texture2D* textures, const iaabb_t* bounds)
{
    for (int y = bounds->min.y; y <= bounds->max.y; y++)
    {
        for (int x = bounds->min.x; x <= bounds->max.x; x++)
        {
            ivec_t pos = {x, y};
            int texture_id = map->ground_views[map_get_idx(map, pos)];
            int x_pos = pos.x * TSIZE;
            int y_pos = pos.y * TSIZE;
            DrawTexture(textures[texture_id], x_pos, y_pos, WHITE);
        }
    }
}

void render_entities(const model_t* model, const Texture2D* textures, const iaabb_t* bounds)
{
    const map_t* map = &model->map;
    int player_count = 0;
    for (int y = bounds->min.y; y <= bounds->max.y; y++)
    {
        for (int x = bounds->min.x; x <= bounds->max.x; x++)
        {
            ivec_t pos = {x, y};
            int idx = map_get_idx(map, pos);
            entity_t entity = map->entities[idx];

            if(entity.type == UNKNOWN_ENTITY) continue;

            int x_pos = pos.x * TSIZE;
            int y_pos = pos.y * TSIZE;
            uint16_t texture_id = entity_get_texture_id(model, entity);

            DrawTexture(textures[texture_id], x_pos, y_pos, WHITE);
        }
    }
}

void render_effects() {}

void render_ingame_ui(){}

void render_ui()
{
    DrawText("HERO FORESTER", 10, 10, 20, WHITE);
}

#endif // RENDER_H
