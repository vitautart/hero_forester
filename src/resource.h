#ifndef RESOURCES_H
#define RESOURCES_H

#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#include "globals.h"

Texture2D* load_textures()
{
    for (int i = 0; i < TEXTURE_ID_MAX_VALUE; i++)
    {
        global_tex_id_right_to_left_mapping[i] = TEXTURE_ID_INVALID;
        global_tex_id_left_to_right_mapping[i] = TEXTURE_ID_INVALID;
    }

    long size = sizeof(Texture2D) * TEXTURE_ID_MAX_VALUE;
    Texture2D* textures = malloc (size);
    memset(textures, 0, size);

    Image player_1_img = LoadImage("assets/player.png");
    Image player_2_img  = ImageCopy(player_1_img);
    ImageFlipHorizontal(&player_2_img);

    Image enemy_1_img = LoadImage("assets/enemy1.png");
    Image enemy_2_img = ImageCopy(enemy_1_img);
    ImageFlipHorizontal(&enemy_2_img);

    Image pathline_horizontal_img = LoadImage("assets/pathline.png");
    Image pathline_vertical_img = ImageCopy(pathline_horizontal_img);
    ImageRotateCW(&pathline_vertical_img);

    Image pathturn_lu_img = LoadImage("assets/pathturn1.png");
    Image pathturn_ru_img = ImageCopy(pathturn_lu_img);
    ImageRotateCW(&pathturn_ru_img);
    Image pathturn_rd_img = ImageCopy(pathturn_ru_img);
    ImageRotateCW(&pathturn_rd_img);
    Image pathturn_ld_img = ImageCopy(pathturn_rd_img);
    ImageRotateCW(&pathturn_ld_img);    

    Image pathturn_tshape_l_img = LoadImage("assets/pathturn2.png");
    Image pathturn_tshape_u_img = ImageCopy(pathturn_tshape_l_img);
    ImageRotateCW(&pathturn_tshape_u_img);
    Image pathturn_tshape_r_img = ImageCopy(pathturn_tshape_u_img);
    ImageRotateCW(&pathturn_tshape_r_img);
    Image pathturn_tshape_d_img = ImageCopy(pathturn_tshape_r_img);
    ImageRotateCW(&pathturn_tshape_d_img);

    textures[TEXTURE_ID_PLAYER_1] = LoadTextureFromImage(player_1_img);
    textures[TEXTURE_ID_PLAYER_2] = LoadTextureFromImage(player_2_img);
    global_tex_id_right_to_left_mapping[TEXTURE_ID_PLAYER_1] = TEXTURE_ID_PLAYER_2;
    global_tex_id_left_to_right_mapping[TEXTURE_ID_PLAYER_2] = TEXTURE_ID_PLAYER_1;

    textures[TEXTURE_ID_ENEMY_1] = LoadTextureFromImage(enemy_1_img);
    textures[TEXTURE_ID_ENEMY_2] = LoadTextureFromImage(enemy_2_img);
    global_tex_id_right_to_left_mapping[TEXTURE_ID_ENEMY_1] = TEXTURE_ID_ENEMY_2;
    global_tex_id_left_to_right_mapping[TEXTURE_ID_ENEMY_2] = TEXTURE_ID_ENEMY_1;

    textures[TEXTURE_ID_PATHLINE_HORIZONTAL] = LoadTextureFromImage(pathline_horizontal_img);
    textures[TEXTURE_ID_PATHLINE_VERICAL] = LoadTextureFromImage(pathline_vertical_img);
    textures[TEXTURE_ID_PATHTURN_LU] = LoadTextureFromImage(pathturn_lu_img);
    textures[TEXTURE_ID_PATHTURN_RU] = LoadTextureFromImage(pathturn_ru_img);
    textures[TEXTURE_ID_PATHTURN_RD] = LoadTextureFromImage(pathturn_rd_img);
    textures[TEXTURE_ID_PATHTURN_LD] = LoadTextureFromImage(pathturn_ld_img);
    textures[TEXTURE_ID_PATHTURN_TSHAPE_L] = LoadTextureFromImage(pathturn_tshape_l_img);
    textures[TEXTURE_ID_PATHTURN_TSHAPE_U] = LoadTextureFromImage(pathturn_tshape_u_img);
    textures[TEXTURE_ID_PATHTURN_TSHAPE_R] = LoadTextureFromImage(pathturn_tshape_r_img);
    textures[TEXTURE_ID_PATHTURN_TSHAPE_D] = LoadTextureFromImage(pathturn_tshape_d_img);
    textures[TEXTURE_ID_PATHTURN_CROSS] = LoadTexture("assets/pathturn3.png");

    textures[TEXTURE_ID_GRASS_1] = LoadTexture("assets/grass.png");
    textures[TEXTURE_ID_TREE_1] = LoadTexture("assets/tree.png");

    // TEXTURES FOR EFFECTS
    textures[TEXTURE_ID_EFFECT_1] = LoadTexture("assets/splash1.png");

    // TEXTURES FOR UI
    textures[TEXTURE_ID_BUTTON_1] = LoadTexture("assets/button_1.png");

    UnloadImage(pathline_horizontal_img);
    UnloadImage(pathline_vertical_img);
    UnloadImage(pathturn_ld_img);
    UnloadImage(pathturn_lu_img);
    UnloadImage(pathturn_rd_img);
    UnloadImage(pathturn_ru_img);
    UnloadImage(pathturn_tshape_d_img);
    UnloadImage(pathturn_tshape_l_img);
    UnloadImage(pathturn_tshape_r_img);
    UnloadImage(pathturn_tshape_u_img);
    UnloadImage(player_1_img);
    UnloadImage(player_2_img);
    UnloadImage(enemy_1_img);
    UnloadImage(enemy_2_img);

    return textures;
}

void unload_textures(Texture2D* textures)
{
    for (int i = 0; i < TEXTURE_ID_MAX_VALUE; i++)
    {
        if (textures[i].width != 0 && textures[i].height != 0)
            UnloadTexture(textures[i]);
    }
    free(textures);
}

#endif // RESOURCES_H
