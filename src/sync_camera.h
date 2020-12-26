#ifndef SYNC_CAMERA_H
#define SYNC_CAMERA_H

#include "presentation.h"
#include "model.h"

#include <raymath.h>

void sync_camera(Camera2D* camera, model_t* model, int screen_w, int screen_h)
{
    ent_player_t* player = GET_PLAYER(model);

    ivec_t pos = map_get_pos(&model->map, player->mapid);

    Vector2 left_up_world = GetScreenToWorld2D((Vector2){0,0}, *camera);
    Vector2 right_bottom_world = GetScreenToWorld2D((Vector2){screen_w,screen_h}, *camera);
    Vector2 world_camera_delta = Vector2Subtract(right_bottom_world, left_up_world);
    Vector2 world_camera_delta_half = Vector2Scale(world_camera_delta, 0.5f); 

    camera->target = map_to_world_c_offset(pos);
    camera->offset = Vector2Scale(world_camera_delta_half, camera->zoom);
}

#endif // SYNC_CAMERA_H
