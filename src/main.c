#include "globals.h"
#include "presentation.h"
#include "raylib.h"
#include "resource.h"
#include "generation.h"
#include "model.h"
#include "render.h"
#include "simulation.h"
#include "uisystem.h"
#include "useractions.h"

//#define OLDSIMULATION

int main (void)
{
    int screen_w = 800;
    int screen_h = 600;
    // Must be first before call any raylib functions
    InitWindow(screen_w, screen_h, "HERO FORESTER");

    Camera2D camera = 
    {
        .rotation = 0.0f,
        .zoom = 2.0f
    };
    model_t model;
    app_state_t app_state = 
    {
        .current_scene = SCENE_TYPE_LOCATION
    };
    
    ui_system_t ui_sys;
    ui_init_system(&ui_sys);

    dynarr_t effects = dynarr_allocate(sizeof(effect_t), 0, 32);
    dynarr_t effect_emmiters = dynarr_allocate(sizeof(effect_emmiter_t), 0, 32);

    globals_allocate_per_session();
    Texture2D* textures = load_textures();
    generate_location(60, 60, &model);

    const ent_player_t* player = (ent_player_t*)dynarr_get(&model.entities[PLAYER_ENTITY], 0);
    entity_t current_entity = model.map.entities[player->mapid];
    action_t current_action;

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {    
        ui_process_system(&ui_sys, &app_state, screen_w, screen_h);

        if (app_state.current_scene == SCENE_TYPE_LOCATION)
        {
            current_action = current_entity.type == PLAYER_ENTITY ? 
                produce_user_action(&model, current_entity, camera, ui_sys.is_mouse_over_map) :
                produce_ai_action(&model, current_entity); 

            current_entity = do_action(&model, &current_action, &effect_emmiters);
            present_convert_for_renderer(&effect_emmiters, &effects);
            present_sync_camera(&camera, &model, screen_w, screen_h);
        }

        render(&camera, &model, app_state.current_scene, &effects, textures, &ui_sys.ui, screen_w, screen_h);

        if (app_state.current_scene == SCENE_TYPE_LOCATION)
        {
            present_mutate_for_renderer(&effects);
        }
    }

    ui_destroy_system(&ui_sys);
    destroy_location(&model);
    globals_free_per_session();
    CloseWindow();
    dynarr_free(&effects);
    dynarr_free(&effect_emmiters);

    return 0;
}
