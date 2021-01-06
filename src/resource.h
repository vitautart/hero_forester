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

    Image pathline_1_img = LoadImage("assets/pathline.png");
    Image pathline_2_img = ImageCopy(pathline_1_img);
    ImageRotateCW(&pathline_2_img);

    textures[TEXTURE_ID_PLAYER_1] = LoadTextureFromImage(player_1_img);
    textures[TEXTURE_ID_PLAYER_2] = LoadTextureFromImage(player_2_img);
    global_tex_id_right_to_left_mapping[TEXTURE_ID_PLAYER_1] = TEXTURE_ID_PLAYER_2;
    global_tex_id_left_to_right_mapping[TEXTURE_ID_PLAYER_2] = TEXTURE_ID_PLAYER_1;

    textures[TEXTURE_ID_ENEMY_1] = LoadTextureFromImage(enemy_1_img);
    textures[TEXTURE_ID_ENEMY_2] = LoadTextureFromImage(enemy_2_img);
    global_tex_id_right_to_left_mapping[TEXTURE_ID_ENEMY_1] = TEXTURE_ID_ENEMY_2;
    global_tex_id_left_to_right_mapping[TEXTURE_ID_ENEMY_2] = TEXTURE_ID_ENEMY_1;

    textures[TEXTURE_ID_GRASS_1] = LoadTexture("assets/grass.png");
    textures[TEXTURE_ID_PATHLINE_1] = LoadTextureFromImage(pathline_1_img);
    textures[TEXTURE_ID_PATHLINE_2] = LoadTextureFromImage(pathline_2_img);
    textures[TEXTURE_ID_PATHTURN_1] = LoadTexture("assets/pathturn.png");
    textures[TEXTURE_ID_TREE_1] = LoadTexture("assets/tree.png");


    // TEXTURES FOR EFFECTS
    textures[TEXTURE_ID_EFFECT_1] = LoadTexture("assets/splash1.png");

    UnloadImage(pathline_1_img);
    UnloadImage(pathline_2_img);
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
