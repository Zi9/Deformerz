#include "Renderer.h"

#include "DFCamera.h"
#include "Engine/Editors/Editors.h"
#include "Engine/Engine.h"
#include "Engine/UI/ImGUI.h"
#include "cimgui.h"
#include <raylib.h>

#define TEREP_GRAPHICS

#ifdef TEREP_GRAPHICS
#define RENDER_W 320
#define RENDER_H 200
#define RENDER_SCL 2
#else
#define RENDER_W 1920
#define RENDER_H 1080
#define RENDER_SCL 1
#endif

RenderTexture2D rendertex;

void Renderer_Initialize()
{
    // InitWindow(1920, 1080, GAME_WINDOW_TITLE);
    InitWindow(RENDER_W * RENDER_SCL, RENDER_H * RENDER_SCL, GAME_WINDOW_TITLE);
    rendertex = LoadRenderTexture(RENDER_W, RENDER_H);
    SetTargetFPS(60);
    rlImGuiSetup(true);
}
void Renderer_Destroy()
{
    UnloadRenderTexture(rendertex);
    rlImGuiShutdown();
    CloseWindow();
}

void Renderer_Render()
{
    BeginDrawing();
    BeginTextureMode(rendertex);
    ClearBackground(Engine.skyColor);
    DFCamera_BeginRender();
    Renderer_RenderMap(Engine.map);
    if (CarEditor_Active) {
        CarEditor_Render3D(Engine.car);
    } else {
        Renderer_RenderCar(Engine.car);
    }
    DFCamera_EndRender();
    EndTextureMode();
    DrawTexturePro(rendertex.texture, (Rectangle){0, 0, rendertex.texture.width, -rendertex.texture.height},
                   (Rectangle){0, 0, RENDER_W * RENDER_SCL, RENDER_H * RENDER_SCL}, (Vector2){0, 0}, 0, WHITE);
    rlImGuiBegin();
    igBeginMainMenuBar();
    if (igBeginMenu("Car Editor", true)) {
        CarEditor_Active = true;
        igEndMenu();
    }
    igEndMainMenuBar();
    CarEditor_RenderUI(Engine.car);
    rlImGuiEnd();
    int height = GetScreenHeight();
    DrawText(WMARK, 0, height - 18, 20, BLACK);
    DrawText(WMARK, 0, height - 20, 20, RED);
    EndDrawing();
}
