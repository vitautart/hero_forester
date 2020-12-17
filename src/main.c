#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "resource.h"
#include "generation.h"
#include "model.h"
#include "render.h"
#include "simulation.h"

Vector2 get_cell_pos(Vector2 world_pos)
{
    return (Vector2)
    {
        .x = (int)world_pos.x / TSIZE - (world_pos.x < 0 ? 1 : 0),
        .y = (int)world_pos.y / TSIZE - (world_pos.y < 0 ? 1 : 0) 
    };
}

Vector2 get_cell_pos_under_cursor(Camera2D camera)
{
    return get_cell_pos(GetScreenToWorld2D(GetMousePosition(), camera));
}

void draw_map_cursor(Vector2 cell_pos)
{
    Rectangle rect = 
    {
        .x = cell_pos.x * TSIZE,
        .y = cell_pos.y * TSIZE,
        .height = TSIZE,
        .width = TSIZE
    };
    DrawRectangleRec(rect, (Color){ .r = 200, .g = 0, .b = 0, .a = 100 });
}

/*int main(void)
{
    int screen_w = 640;
    int screen_h = 480;

    int player_tex_id = 0;
    Vector2 player_pos = { .x = TSIZE * 8, .y = TSIZE * 8 };

    const char* title = "HERO FORESTER!!!";
    
    Camera2D camera = 
    {
        .offset = player_pos,
        .target = player_pos,
        .rotation = 0.0f,
        .zoom = 2.0f
    };

    InitWindow(screen_w, screen_h, "HERO FORESTER");

    Texture2D* textures = NULL;
    int texture_count;

    SetTargetFPS(60);
    while(!WindowShouldClose())
    { 
        Vector2 cursor_cell_pos = get_cell_pos_under_cursor(camera);
        Vector2 player_cell_pos = get_cell_pos(player_pos);

        camera.target = player_pos;


        BeginDrawing();
        ClearBackground((Color){ .r = 0, .g = 100, .b = 0, .a = 255 });
        {
            BeginMode2D(camera);
            draw_tiled_background(tiling_textures, layout);
            DrawTextureEx(textures[player_tex_id], player_pos, 0, 1, WHITE);
            for (int i = 0; i < success; i++)
                DrawTexture(textures[1], (int)positions[i].x, (int)positions[i].y, WHITE);
            draw_map_cursor(cursor_cell_pos);
            EndMode2D();
        }
        DrawText(title, 10, 10, 20, WHITE);
        EndDrawing();
    }

    free_tiling_layout(layout);
    unload_resources(textures, texture_count);
    free(positions);
    CloseWindow();
    return 0;
}*/

int main (void)
{
    Camera2D camera = 
    {
        .offset = {10, 10},
        .target = {10, 10},
        .rotation = 0.0f,
        .zoom = 2.0f
    };
    entity_container_t container;
    map_t map;
    round_t round = ROUND_WAIT;
    InitWindow(800, 600, "HERO FORESTER");
    Texture2D* textures = load_textures();
    generate_location(500, 500, &container, &map);

    SetTargetFPS(60);
    while(!WindowShouldClose())
    { 
        round = simulate(&container, &map, round);
        render(camera, &map, &container, textures);
    }
    destroy_location(&container, &map);
    CloseWindow();
    return 0;
}
