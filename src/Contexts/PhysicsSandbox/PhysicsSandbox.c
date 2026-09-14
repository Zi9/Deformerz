#include "PhysicsSandbox.h"
#include "Engine/DFCar.h"
#include "Engine/DFMap.h"
#include "Engine/UI.h"
#include "FreeCam.h"

DFCar* car;
DFMap* map;

static void _DoPhysicsCalculations()
{
    // TODO: Implement epic physics simulation here :)
}

static void _OnEnter()
{
    car = DFCar_Load("car1");
    map = DFMap_Load();
    FreeCam_SetPos((Vector3){-1, 0.5, -1});
    FreeCam_SetRot((Vector2){45, -15});
}

static void _OnExit()
{
    DFCar_Unload(car);
    DFMap_Unload(map);
}

static void _Run()
{
    FreeCam_Update();

    _DoPhysicsCalculations();

    BeginDrawing();
    ClearBackground(Engine_Palette[255]);

    FreeCam_BeginRender();
    DFMap_Render(map);
    DFCar_Render(car);
    FreeCam_EndRender();

    rlImGuiBegin();
    igSetNextWindowSize((ImVec2){400, 400}, ImGuiCond_Once);
    igBegin("Deformerz Physics Sandbox", NULL, ImGuiWindowFlags_NoCollapse);
    igText("Delta Time: %f\n", GetFrameTime());
    // TODO: Use this to show debug data
    igEnd();
    rlImGuiEnd();

    EndDrawing();
}

EngineContext PhysicsSandboxContext() { return (EngineContext){.Run = _Run, .OnEnter = _OnEnter, .OnExit = _OnExit}; }
