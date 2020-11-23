#include <raylib.h>
#include <stdio.h>

int main(void)
{
    InitWindow(640, 480, "HERO FORESTER");

    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

