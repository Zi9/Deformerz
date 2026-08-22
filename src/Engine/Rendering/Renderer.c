#include "Renderer.h"

#include "DFCamera.h"
#include "Engine/Debug/Debug.h"
#include "Engine/Engine.h"
#include "Engine/UI/ImGUI.h"
#include <raylib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define R2DW 640
#define R2DH 480
#define BARH 17

void Renderer_Initialize()
{
    InitWindow(1920, 1080, GAME_WINDOW_TITLE);
    SetTargetFPS(60);
    rlImGuiSetup(true);
    Engine.skyColor = (Color){40, 40, 40, 255};
}
void Renderer_Destroy()
{
    rlImGuiShutdown();
    CloseWindow();
}

void Renderer_Render()
{
    BeginDrawing();
    ClearBackground(Engine.skyColor);
    DFCamera_BeginRender();
    Renderer_RenderCar(Engine.car);
    DFCamera_EndRender();
    rlImGuiBegin();
    //Debug_RenderCarDebugger(Engine.car);
    rlImGuiEnd();
    int height = GetScreenHeight();
    DrawText(WMARK, 0, height - 18, 20, BLACK);
    DrawText(WMARK, 0, height - 20, 20, RED);
    EndDrawing();
}
