#include "Engine.h"
#include "Rendering/DFCamera.h"
#include "Rendering/Renderer.h"
#include <raylib.h>

EngineData Engine = {0};

void Engine_Initialize()
{
    SetTraceLogLevel(LOG_ERROR);
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    Renderer_Initialize();
    SetTargetFPS(60);
    Engine.map = DFMap_Load();
    Engine.car = DFCar_Load();
    DFCamera_SetPos((Vector3) {0, 0, 3});
}

void Engine_Loop()
{
    Engine.dt = GetFrameTime();
    Engine.time += Engine.dt;
    DFCamera_Update();
    Renderer_Render();
}

void Engine_Destroy()
{
    Renderer_Destroy();
    DFCar_Unload(Engine.car);
    DFMap_Unload(Engine.map);
}

void Engine_Main()
{
    Engine_Initialize();
    while (!WindowShouldClose()) {
        Engine_Loop();
    }
    Engine_Destroy();
}
