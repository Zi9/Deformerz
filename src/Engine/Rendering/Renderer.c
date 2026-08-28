#include "Renderer.h"

#include "DFCamera.h"
#include "Engine/Editors/CarEditor.h"
#include "Engine/Editors/Editors.h"
#include "Engine/Engine.h"
#include "Engine/UI/ImGUI.h"
#include <cimgui.h>
#include <raylib.h>

//#define TEREP_GRAPHICS
#define BIGPIXEL_GRAPHICS

#ifdef TEREP_GRAPHICS
#define RENDER_W 320
#define RENDER_H 200
#define RENDER_SCL 2
#elifdef BIGPIXEL_GRAPHICS
#define RENDER_W 960
#define RENDER_H 540
#define RENDER_SCL 2
#else
#define RENDER_W 1920
#define RENDER_H 1080
#define RENDER_SCL 1
#endif

RenderTexture2D rendertex;

Color Renderer_Palette[256] = {0};
Color Renderer_SkyColor = GRAY;

void Renderer_Initialize()
{
    // InitWindow(1920, 1080, GAME_WINDOW_TITLE);
    InitWindow(RENDER_W * RENDER_SCL, RENDER_H * RENDER_SCL, GAME_WINDOW_TITLE);
    rendertex = LoadRenderTexture(RENDER_W, RENDER_H);
    SetTargetFPS(60);
    rlImGuiSetup(true);
    PCX_EnableGlobalPalette("./data/col.pcx"); // HACK: Fix this hardcoding at some point
    for (size_t i = 0; i < 256; i++)
    {
        Renderer_Palette[i].r = PCX_GLOBAL_PALETTE[i].red;
        Renderer_Palette[i].g = PCX_GLOBAL_PALETTE[i].green;
        Renderer_Palette[i].b = PCX_GLOBAL_PALETTE[i].blue;
        Renderer_Palette[i].a = 255;
    }
    Renderer_SkyColor = Renderer_Palette[255];
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
    ClearBackground(Renderer_SkyColor);
    DFCamera_BeginRender();
    Render_DFMap(Engine.map);
    if (CarEditor.Active) {
        CarEditor_Render3D(Engine.car);
    } else {
        Render_DFCar(Engine.car);
    }
    DFCamera_EndRender();
    EndTextureMode();
    DrawTexturePro(rendertex.texture, (Rectangle){0, 0, rendertex.texture.width, -rendertex.texture.height},
                   (Rectangle){0, 0, RENDER_W * RENDER_SCL, RENDER_H * RENDER_SCL}, (Vector2){0, 0}, 0, WHITE);
    rlImGuiBegin();
    igBeginMainMenuBar();
    if (igBeginMenu("Car Editor", true)) {
        CarEditor.Active = true;
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
