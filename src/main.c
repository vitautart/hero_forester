#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float norm_rand()
{
    return (RAND_MAX - rand()) / (float)RAND_MAX;
}

void load_resources (Texture2D** textures, int* texture_count)
{
    *texture_count = 2;
    *textures = malloc(sizeof(Texture2D) * (*texture_count));

    (*textures)[0] = LoadTexture("assets/uncle.png");
    (*textures)[1] = LoadTexture("assets/tree.png");
}

void unload_resources (Texture2D* textures, int texture_count)
{
    for (int i = 0; i < texture_count; i++)
        UnloadTexture(textures[i]);
    free(textures);
}

int main(void)
{
    int screen_w = 640;
    int screen_h = 480;

    Vector2 player_pos = { .x = screen_w * 0.5f, .y = screen_h * 0.5f };
    const int entity_count = 30;
    Vector2* positions = malloc(sizeof(Vector2) * entity_count);
    float tree_rand_radius = 100;
    time_t t;
    srand((unsigned) time(&t));
    for (int i = 0; i < 30; i++)
        positions[i] = (Vector2){ .x = (int)player_pos.x + 2 * (norm_rand() - 0.5) * tree_rand_radius, .y = (int)player_pos.y + 2 *(norm_rand() - 0.5) * tree_rand_radius};

    const char* title = "HERO FORESTER!!!";
    
    Camera2D camera = 
    {
        .offset = player_pos,
        .target = player_pos,
        .rotation = 0.0f,
        .zoom = 3.0f
    };

    InitWindow(screen_w, screen_h, "HERO FORESTER");

    Texture2D* textures = NULL;
    int texture_count;

    load_resources(&textures, &texture_count);

    SetTargetFPS(60);
    printf("dfgdfg");
    while(!WindowShouldClose())
    {
        if (IsKeyDown(KEY_LEFT))    player_pos.x -= 1.0f;
        if (IsKeyDown(KEY_RIGHT))   player_pos.x += 1.0f;
        if (IsKeyDown(KEY_UP))      player_pos.y -= 1.0f;
        if (IsKeyDown(KEY_DOWN))    player_pos.y += 1.0f;

        BeginDrawing();
        ClearBackground((Color){ .r = 0, .g = 100, .b = 0, .a = 255 });
        {
            BeginMode2D(camera);
            DrawTexture(textures[0], (int)player_pos.x, (int)player_pos.y, WHITE);
            for (int i = 0; i < entity_count; i++)
                DrawTexture(textures[1], (int)positions[i].x, (int)positions[i].y, WHITE);
            EndMode2D();
        }
        DrawText(title, 10, 10, 20, WHITE);
        EndDrawing();
    }

    unload_resources(textures, texture_count);
    free(positions);
    CloseWindow();
    return 0;
}

