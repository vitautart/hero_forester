#include "resource.h"
#include "generation.h"
#include "model.h"
#include "render.h"
#include "simulation.h"
#include "sync_camera.h"

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
        .rotation = 0.0f,
        .zoom = 2.0f
    };
    int screen_w = 800;
    int screen_h = 600;
    model_t model;
    round_t round = ROUND_WAIT;
    InitWindow(screen_w, screen_h, "HERO FORESTER");
    Texture2D* textures = load_textures();
    generate_location(500, 500, &model);

    SetTargetFPS(60);
    while(!WindowShouldClose())
    { 
        sync_camera(&camera, &model, screen_w, screen_h);
        round = simulate(&model, round);
        render(camera, &model, textures, screen_w, screen_h);
    }
    destroy_location(&model);
    CloseWindow();
    return 0;
}
