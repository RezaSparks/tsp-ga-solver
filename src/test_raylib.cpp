#include <raylib.h>
#include <cstdio>

int main() {
    InitWindow(800, 600, "Raylib Test");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RED);          // پس‌زمینه قرمز
        DrawCircle(400, 300, 100, BLUE);
        DrawText("If you see red + blue circle, Raylib works!", 150, 500, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}