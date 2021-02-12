#ifndef RENDER_H
#define RENDER_H

#include "effects.h"
#include "globals.h"
#include "presentation.h"
#include "model.h"
#include "raylib.h"
#include "raymath.h"
#include "uisystem.h"
#include <stdio.h>
#include <sys/types.h>

iaabb_t camera_get_map_boundaries(Camera2D camera, int screen_w, int screen_h);
iaabb_t calculate_draw_bounds(Camera2D camera, const map_t* map, int screen_w, int screen_h, int safety_extent);
void render_cell_with_rectangle(ivec_t pos, int has_border,
        int has_fill, int border_thickness, Color fill_color, Color border_color);
void render(const Camera2D* camera, const model_t* model, const scene_type_t scene, /*const user_state_t* user_state,*/ const dynarr_t* effects, const Texture2D *textures, const ui_t* ui, int screen_w, int screen_h);
void render_ground(const map_t * map, const Texture2D* textures, const iaabb_t* bounds);
void render_entities(const model_t* model, const Texture2D* textures, const iaabb_t* bounds);
void render_effects(const dynarr_t* effects, const Texture2D *textures);
void render_ingame_ui(Camera2D camera, const model_t* model/*, const user_state_t* user_state*/);
void render_cell_under_cursor(Camera2D camera, const model_t* model/*,
        const user_state_t* user_state*/);
void render_ui(const ui_t* container, const Texture2D* textures);

#ifdef DEBUG_RENDER
void render_debug_cells(const map_t * map)
{
    for (int i = 0; i < global_debug_red_map_cell_pos.size; i++)
    {
        ivec_t pos = GET_AS(ivec_t, dynarr_get(&global_debug_red_map_cell_pos, i));
        render_cell_with_rectangle(pos, 0, 1, 0, (Color){200, 0, 0, 100}, (Color){});
    }

    for (int i = 0; i < global_debug_blue_map_cell_pos.size; i++)
    {
        ivec_t pos = GET_AS(ivec_t, dynarr_get(&global_debug_blue_map_cell_pos, i));
        render_cell_with_rectangle(pos, 0, 1, 0, (Color){0, 0, 200, 100}, (Color){});
    }
    
    for (int i = 0; i < global_debug_grey_map_cell_pos.size; i++)
    {
        ivec_t pos = GET_AS(ivec_t, dynarr_get(&global_debug_grey_map_cell_pos, i));
        render_cell_with_rectangle(pos, 0, 1, 0, (Color){100, 100, 100, 200}, (Color){});
    }

    for (int i = 0; i < global_debug_yellow_map_cell_pos.size; i++)
    {
        ivec_t pos = GET_AS(ivec_t, dynarr_get(&global_debug_yellow_map_cell_pos, i));
        render_cell_with_rectangle(pos, 0, 1, 0, (Color){240, 252, 83, 100}, (Color){});
    }
}
#endif

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

void render_cell_under_cursor(Camera2D camera, const model_t* model/*,
        const user_state_t* user_state*/)
{
    ivec_t pos = map_get_mouse_pos(camera);
    //if (user_state->command_mode == MOVE_USER_MODE)
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

void render(const Camera2D* camera, const model_t* model, const scene_type_t scene_type, /*const user_state_t* user_state,*/ const dynarr_t* effects, const Texture2D *textures, const ui_t* ui, int screen_w, int screen_h)
{
    BeginDrawing();
    ClearBackground((Color){ .r = 0, .g = 0, .b = 0, .a = 255 });
    if (scene_type == SCENE_TYPE_LOCATION)
    {
        BeginMode2D(*camera);
        iaabb_t bounds = calculate_draw_bounds(*camera, &model->map, screen_w, screen_h, 2);
        render_ground(&model->map, textures, &bounds);
        render_entities(model, textures, &bounds);
        render_effects(effects, textures);
        render_ingame_ui(*camera, model/*, user_state*/);
#ifdef DEBUG_RENDER
        render_debug_cells(&model->map);
#endif
        EndMode2D();
    }
    render_ui(ui, textures);

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

void render_effects(const dynarr_t* effects, const Texture2D* textures) 
{   
    if (effects->size == 0) return;

    effect_t* effect = effects->data;

    for (int i = 0; i < effects->size; i++)
    {
        switch (effect->type)
        {
            case EFFECT_TYPE_SPLASH: 
                {
                    Vector2 pos = map_to_world_lu_offset(effect->splash.pos); 
                    DrawTextureV(textures[effect->splash.texture_id], pos, WHITE);
                    break;
                }
            case EFFECT_TYPE_PROJECTILE: 
                {
                    Vector2 start = map_to_world_c_offset(effect->projectile.start); 
                    Vector2 end = map_to_world_c_offset(effect->projectile.end);
                    Vector2 dir = Vector2Normalize(Vector2Subtract(end, start));
                    start = Vector2Add(start, Vector2Scale(dir, 16));
                    DrawLineEx(start, end, 1, RED);
                    break;
                }
        }
        effect++;
    }
}

void render_ingame_ui(Camera2D camera, const model_t* model/*, const user_state_t* user_state*/)
{
    render_cell_under_cursor(camera, model/*, user_state*/);
}

// TODO: can be added camera culling
void render_ui(const ui_t* ui, const Texture2D* textures)
{
    //DrawText("HERO FORESTER", 10, 10, 20, WHITE);
    const dynarr_t* layers = ui->data.data;

    for (int i = 0; i < ui->data.size; i++)
    {
        const dynarr_t* layer_arr = layers + i;
        const ui_entity_t* layer = layer_arr->data;
        for (int entity_id = 0; entity_id < layer_arr->size; entity_id++)
        {
            const ui_entity_t* e = &layer[entity_id];
            if (e->is_visible == 0) continue;
            if (e->type == UI_TYPE_CANVAS) continue;
            Rectangle r = 
            {   
                .x = e->screen_pos.x,
                .y = e->screen_pos.y,
                .width = e->size.x,
                .height = e->size.y
            };

            switch (e->type)
            {
                case UI_TYPE_PANEL:
                    {
                        DrawRectangleRec(r, e->panel.background_color);
                        if (e->panel.has_borders) 
                            DrawRectangleLinesEx(r, 1, e->panel.border_color);
                        break;
                    };
                case UI_TYPE_LABEL: 
                    {
                        DrawText(e->label.text, 
                                e->screen_pos.x, 
                                e->screen_pos.y, 
                                e->size.y, 
                                e->label.text_color);
                        break;
                    }
                case UI_TYPE_IMAGE: 
                    {
                        DrawTextureQuad(textures[e->image.tex_id], 
                                Vector2One(), Vector2Zero(), r, WHITE);
                        break;
                    }
                case UI_TYPE_PANEL_9_PATCH: 
                    {
                        NPatchInfo patch = 
                        {
                            .left = e->panel_9_patch.left,
                            .right = e->panel_9_patch.right,
                            .top = e->panel_9_patch.top,
                            .bottom = e->panel_9_patch.bottom,
                            .sourceRec = 
                            {
                                .x = 0,
                                .y = 0,
                                .width = 16,
                                .height = 16
                            },
                            .type = NPT_9PATCH,
                        };
                        DrawTextureNPatch(textures[e->panel_9_patch.tex_id],
                                patch, r, Vector2Zero(), 0, e->panel_9_patch.tint);
                        break;
                    }
                case UI_TYPE_BUTTON: break;
                case UI_TYPE_CANVAS: break; // just for consistency
            };
        }
    }
}

#endif // RENDER_H
