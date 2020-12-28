#include "common.h"
#include "resource.h"
#include "generation.h"
#include "model.h"
#include "render.h"
#include "simulation.h"
#include "sync_camera.h"

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
    InitWindow(screen_w, screen_h, "HERO FORESTER");
    Texture2D* textures = load_textures();
    generate_location(500, 500, &model);

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

        current_entity = do_action(&model, &current_action);

        sync_camera(&camera, &model, screen_w, screen_h);

        render(camera, &model, &user_state, textures, screen_w, screen_h);
    }
    destroy_location(&model);
    CloseWindow();
    return 0;
}
