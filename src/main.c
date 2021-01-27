#include "common.h"
#include "effects.h"
#include "globals.h"
#include "presentation.h"
#include "resource.h"
#include "generation.h"
#include "model.h"
#include "render.h"
#include "simulation.h"
#include "sync_camera.h"
#include "useractions.h"

//#define OLDSIMULATION

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
    ui_container_t ui = ui_allocate(8, 32);

    // TESTING
    {

        ui_entity_t p = 
        {
            .base_align = {1, 0},
            .base_pos = {-20, 20},
            .size = {100, 100},
            .is_visible = 1,
            .type = UI_TYPE_PANEL,
            .parent_align = {1, 0},
            .panel = 
            {
                .has_borders = 1,
                .backgroud_color = {125, 0, 0, 255},
                .border_color = {0, 125, 0, 255}
            }
        };
        ui_entity_t p_child = 
        {
            .base_align = {1, 1},
            .base_pos = {-10, -10},
            .size = {30, 30},
            .is_visible = 1,
            .type = UI_TYPE_PANEL,
            .parent_align = {1, 1},
            .panel = 
            {
                .has_borders = 1,
                .backgroud_color = {125, 0, 125, 255},
                .border_color = {0, 125, 255, 255}
            }
        };
        ui_entity_t* e = ui_add_child(&ui, NULL, &p);
        ui_add_child(&ui, e, &p_child);
    }

    dynarr_t effects = dynarr_allocate(sizeof(effect_t), 0, 32);
    dynarr_t effect_emmiters = dynarr_allocate(sizeof(effect_emmiter_t), 0, 32);
    InitWindow(screen_w, screen_h, "HERO FORESTER");

    globals_allocate_per_session();
    Texture2D* textures = load_textures();
    generate_location(60, 60, &model);

    const ent_player_t* player = (ent_player_t*)dynarr_get(&model.entities[PLAYER_ENTITY], 0);
    entity_t current_entity = model.map.entities[player->mapid];
    action_t current_action;

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {    
        // TODO:    hardcoded default value for player active mode MOVE_USER_MODE is not OK.
        //          we must change this to some active tool in user ui that corresponds to certain user mode.
        user_state.command_mode = current_entity.type != PLAYER_ENTITY ? 
            WAIT_FOR_OTHERS_USER_MODE : MOVE_USER_MODE;

        current_action = current_entity.type == PLAYER_ENTITY ? 
            produce_user_action(&model, current_entity, &user_state, camera)
            : produce_ai_action(&model, current_entity); 

        ui_process(&ui, screen_w, screen_h);
        current_entity = do_action(&model, &current_action, &effect_emmiters);
        convert_data_for_renderer(&effect_emmiters, &effects);

        sync_camera(&camera, &model, screen_w, screen_h);

        render(camera, &model, &user_state, &effects, textures, &ui, screen_w, screen_h);
        mutate_data_for_renderer(&effects);
    }

    ui_free(&ui);
    destroy_location(&model);
    globals_free_per_session();
    CloseWindow();
    dynarr_free(&effects);
    dynarr_free(&effect_emmiters);
    return 0;
}
