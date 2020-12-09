#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "globals.h"

typedef struct tiling_layout_t
{
    int *data;
    int width;
    int height;
} tiling_layout_t;

typedef struct bullet_effect_t
{
    Vector2 start;
    Vector2 end;
    float interpolator;
} bullet_effect_t;

int get_tiling_layout_cell(tiling_layout_t* layout, int x, int y)
{
    return layout->data[x + y * layout->width];
}

tiling_layout_t create_tiling_layout(int width, int height)
{
    tiling_layout_t l = {.width = width, .height = height};
    int size_in_bytes = width * height * sizeof(int);
    l.data = malloc(size_in_bytes);
    memset(l.data, 0, size_in_bytes);
    return l;
}

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

void draw_bullet_effect(bullet_effect_t * effect, Texture2D bullet_texture)
{
    Vector2 pos = Vector2Lerp(effect->start, effect->end, effect->interpolator);
    DrawTextureV(bullet_texture, pos, WHITE);
}

void free_tiling_layout(tiling_layout_t layout)
{
    free(layout.data);
}

float norm_rand()
{
    return (RAND_MAX - rand()) / (float)RAND_MAX;
}

void load_resources (Texture2D** textures, int* texture_count)
{
    *texture_count = 5;
    *textures = malloc(sizeof(Texture2D) * (*texture_count));

    Image img_right = LoadImage("assets/uncle2.png");
    Image img_left  = ImageCopy(img_right);
    ImageFlipHorizontal(&img_left);

    (*textures)[0] = LoadTextureFromImage(img_right);
    (*textures)[1] = LoadTexture("assets/tree.png");
    (*textures)[2] = LoadTexture("assets/grass.png");
    (*textures)[3] = LoadTextureFromImage(img_left);
    (*textures)[4] = LoadTexture("assets/bullet.png");

    UnloadImage(img_right);
    UnloadImage(img_left);
}

void unload_resources (Texture2D* textures, int texture_count)
{
    for (int i = 0; i < texture_count; i++)
        UnloadTexture(textures[i]);
    free(textures);
}

void draw_tiled_background(Texture2D* textures, tiling_layout_t layout)
{
    for (int x = 0; x < layout.width; x++)
    {
        for (int y = 0; y < layout.height; y++)
        {
            int id = get_tiling_layout_cell(&layout, x, y);
            int x_pos = x * TSIZE;
            int y_pos = y * TSIZE;
            DrawTexture(textures[id], x_pos, y_pos, WHITE);
        }
    }
}

int main(void)
{
    int screen_w = 640;
    int screen_h = 480;

    int player_tex_id = 0;
    Vector2 player_pos = { .x = TSIZE * 8, .y = TSIZE * 8 };
    const int entity_count = 1500;
    Vector2* positions = malloc(sizeof(Vector2) * entity_count);
    time_t t;
    srand((unsigned) time(&t));
    int success = 0;
    for (int y = 0; y < 50; y++)
    {
        for (int x = 0; x < 50; x++)
        {
            if (norm_rand() > 0.5)
            {
                positions[success] = (Vector2){.x = TSIZE * x, .y = TSIZE * y}; 
                success++;
            }
            if (success == entity_count)
                break;
        }
        if (success == entity_count)
                break;
    }

    const char* title = "HERO FORESTER!!!";
    
    Camera2D camera = 
    {
        .offset = player_pos,
        .target = player_pos,
        .rotation = 0.0f,
        .zoom = 2.0f
    };

    tiling_layout_t layout = create_tiling_layout(50, 50);


    InitWindow(screen_w, screen_h, "HERO FORESTER");

    Texture2D* textures = NULL;
    int texture_count;

    load_resources(&textures, &texture_count);
    Texture2D tiling_textures [] = {textures[2]};


    SetTargetFPS(60);
    while(!WindowShouldClose())
    { 
        Vector2 cursor_cell_pos = get_cell_pos_under_cursor(camera);
        Vector2 player_cell_pos = get_cell_pos(player_pos);

        if (IsKeyPressed(KEY_LEFT))
        {
            player_tex_id = 3;
            player_pos.x -= 1.0f * TSIZE;
        }
        if (IsKeyPressed(KEY_RIGHT))
        {
            player_tex_id = 0;
            player_pos.x += 1.0f * TSIZE;
        }
        if (IsKeyPressed(KEY_UP))      player_pos.y -= 1.0f * TSIZE;
        if (IsKeyPressed(KEY_DOWN))    player_pos.y += 1.0f * TSIZE;
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
}

