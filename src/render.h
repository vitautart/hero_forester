#ifndef RENDER_H
#define RENDER_H

#include "common.h"
#include "presentation.h"
#include "model.h"
#include "raylib.h"
#include <sys/types.h>

iaabb_t camera_get_map_boundaries(Camera2D camera, int screen_w, int screen_h);
iaabb_t calculate_draw_bounds(Camera2D camera, const map_t* map, int screen_w, int screen_h, int safety_extent);
void render(Camera2D camera, const model_t* model, const user_state_t* user_state, const Texture2D *textures, int screen_w, int screen_h);
void render_ground(const map_t * map, const Texture2D* textures, const iaabb_t* bounds);
void render_entities(const model_t* model, const Texture2D* textures, const iaabb_t* bounds);
void render_effects();
void render_ingame_ui(Camera2D camera, const model_t* model, const user_state_t* user_state);
void render_cell_under_cursor(Camera2D camera, const model_t* model,
        const user_state_t* user_state);
void render_ui();

void render_cell_with_rectangle(ivec_t pos, int has_border,
        int has_fill, int border_thickness, Color fill_color, Color border_color)
{
    Rectangle rect = 
    {
        .x = pos.x * TSIZE,
        .y = pos.y * TSIZE,
        .height = TSIZE,
        .width = TSIZE
    };
    if (has_fill)   DrawRectangleRec(rect, fill_color);
    if (has_border) DrawRectangleLinesEx(rect, border_thickness, border_color);
}

void render_cell_under_cursor(Camera2D camera, const model_t* model,
        const user_state_t* user_state)
{
    ivec_t pos = world_to_map(GetScreenToWorld2D(GetMousePosition(), camera));
    if (user_state->command_mode == MOVE_USER_MODE)
    {
        int map_idx = map_get_idx(&model->map, pos);
        Color color = { .r = 0, .g = 0, .b = 0, .a = 100 };
        if (map_check_pos_outside(&model->map, pos) 
            || bitset_get(&model->map.obstacles, map_idx))
            color.r = 200;
        else
            color.b = 200;

        render_cell_with_rectangle(pos, 0, 1, 0, color, color);
    }
}

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

void render(Camera2D camera, const model_t* model, const user_state_t* user_state, const Texture2D *textures, int screen_w, int screen_h)
{
    iaabb_t bounds = calculate_draw_bounds(camera, &model->map, screen_w, screen_h, 2);
    BeginDrawing();
    ClearBackground((Color){ .r = 0, .g = 0, .b = 0, .a = 255 });
    
    BeginMode2D(camera);
    render_ground(&model->map, textures, &bounds);
    render_entities(model, textures, &bounds);
    render_effects();
    render_ingame_ui(camera, model, user_state);
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

void render_ingame_ui(Camera2D camera, const model_t* model, const user_state_t* user_state)
{
    render_cell_under_cursor(camera, model, user_state);
}

void render_ui()
{
    DrawText("HERO FORESTER", 10, 10, 20, WHITE);
}

#endif // RENDER_H
