#include "Renderer.h"

#include "DFCamera.h"
#include "Engine/Engine.h"
#include "Engine/UI/ImGUI.h"
#include <raylib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define R2DW 640
#define R2DH 480
#define BARH 17

void Renderer_Initialize()
{
    InitWindow(Engine.cfg.render.width, Engine.cfg.render.height, GAME_WINDOW_TITLE);
    SetTargetFPS(Engine.cfg.render.targetFPS);
    rlImGuiSetup(true);
}
void Renderer_Destroy()
{
    rlImGuiShutdown();
    CloseWindow();
}

void Renderer_Render()
{
    BeginDrawing();
    ClearBackground(Engine.cfg.skyColor);
    DFCamera_BeginRender();
    Renderer_RenderDFMap(Engine.map);
    Renderer_RenderDFCar(Engine.car);
    DFCamera_EndRender();
    rlImGuiBegin();
    if (igBeginMainMenuBar()) {
        igEndMainMenuBar();
    }
    rlImGuiEnd();
    int height = GetScreenHeight();
    DrawText(WMARK, 0, height - 18, 20, BLACK);
    DrawText(WMARK, 0, height - 20, 20, RED);
    EndDrawing();
}
