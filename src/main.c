#include "resource.h"
#include "generation.h"
#include "model.h"
#include "render.h"
#include "simulation.h"
#include "sync_camera.h"

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
    user_state_t user_state = {.command_mode = MOVE_USER_MODE};
    round_t round = ROUND_WAIT;
    InitWindow(screen_w, screen_h, "HERO FORESTER");
    Texture2D* textures = load_textures();
    generate_location(500, 500, &model);

    SetTargetFPS(60);
    while(!WindowShouldClose())
    { 
        sync_camera(&camera, &model, screen_w, screen_h);
        round = simulate(&model, &user_state, round);
        render(camera, &model, &user_state, textures, screen_w, screen_h);
    }
    destroy_location(&model);
    CloseWindow();
    return 0;
}
