#ifndef RESOURCES_H
#define RESOURCES_H

#include <stdlib.h>
#include <string.h>
#include <raylib.h>

#include "globals.h"

Texture2D* load_textures()
{
    long size = sizeof(Texture2D) * TEXTURE_ID_MAX_VALUE;
    Texture2D* textures = malloc (size);
    memset(textures, 0, size);

    Image player_1_img = LoadImage("assets/player.png");
    Image player_2_img  = ImageCopy(player_1_img);
    ImageFlipHorizontal(&player_2_img);

    Image enemy_1_img = LoadImage("assets/enemy1.png");
    Image enemy_2_img = ImageCopy(enemy_1_img);
    ImageFlipHorizontal(&enemy_2_img);

    textures[TEXTURE_ID_PLAYER_1] = LoadTextureFromImage(player_1_img);
    textures[TEXTURE_ID_PLAYER_2] = LoadTextureFromImage(player_2_img);
    textures[TEXTURE_ID_ENEMY_1] = LoadTextureFromImage(enemy_1_img);
    textures[TEXTURE_ID_ENEMY_2] = LoadTextureFromImage(enemy_2_img);

    textures[TEXTURE_ID_GRASS_1] = LoadTexture("assets/grass.png");
    textures[TEXTURE_ID_PATHLINE_1] = LoadTexture("assets/pathline.png");
    textures[TEXTURE_ID_TREE_1] = LoadTexture("assets/tree.png");

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
