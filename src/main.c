#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct tiling_layout_t
{
    int *data;
    int width;
    int height;
} tiling_layout_t;

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
    *texture_count = 3;
    *textures = malloc(sizeof(Texture2D) * (*texture_count));

    (*textures)[0] = LoadTexture("assets/uncle.png");
    (*textures)[1] = LoadTexture("assets/tree.png");
    (*textures)[2] = LoadTexture("assets/grass.png");
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
            int x_pos = x * 32;
            int y_pos = y * 32;
            DrawTexture(textures[id], x_pos, y_pos, WHITE);
        }
    }
}

int main(void)
{
    int screen_w = 640;
    int screen_h = 480;

    Vector2 player_pos = { .x = screen_w * 0.5f, .y = screen_h * 0.5f };
    const int entity_count = 300;
    Vector2* positions = malloc(sizeof(Vector2) * entity_count);
    time_t t;
    srand((unsigned) time(&t));
    int success = 0;
    for (int y = 0; y < 50; y++)
    {
        for (int x = 0; x < 50; x++)
        {
            if (norm_rand() > 0.9)
            {
                positions[success] = (Vector2){.x = 32 * x, .y = 32 * y}; 
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
        if (IsKeyDown(KEY_LEFT))    player_pos.x -= 1.0f;
        if (IsKeyDown(KEY_RIGHT))   player_pos.x += 1.0f;
        if (IsKeyDown(KEY_UP))      player_pos.y -= 1.0f;
        if (IsKeyDown(KEY_DOWN))    player_pos.y += 1.0f;

        BeginDrawing();
        ClearBackground((Color){ .r = 0, .g = 100, .b = 0, .a = 255 });
        {
            camera.target = player_pos;
            BeginMode2D(camera);
            draw_tiled_background(tiling_textures, layout);
            DrawTextureEx(textures[0], player_pos, 0, 1, WHITE);
            for (int i = 0; i < success; i++)
                DrawTexture(textures[1], (int)positions[i].x, (int)positions[i].y, WHITE);
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

