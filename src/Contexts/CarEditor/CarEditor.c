#include "CarEditor.h"
#include "EditorCam.h"
#include "Engine/Engine.h"
#include "Engine/UI.h"
#include "raylib.h"

static CarEditorMode _mode = {0};
static CarEditorMode _nextMode = {0};
static bool _hasNextMode = false;
static DFCar* _car;
static Color _bg3d = (Color){40, 40, 40, 255};

static bool _quadView = false;

static RenderTexture _viewport;
static RenderTexture _viewportQ1;
static RenderTexture _viewportQ2;
static RenderTexture _viewportQ3;
static RenderTexture _viewportQ4;

static void _Enter()
{
    _mode = CarEditorMode_Meta();
    _car = DFCar_Load();
    EditorCam_SetPosRot((Vector3){-0.75, 0.6, -0.75}, (Vector2){40, -30});
    _viewport = LoadRenderTexture(1280/2, 1080/2);
    _viewportQ1 = LoadRenderTexture(638, 538);
    _viewportQ2 = LoadRenderTexture(638, 538);
    _viewportQ3 = LoadRenderTexture(638, 538);
    _viewportQ4 = LoadRenderTexture(638, 538);
}
static void _Exit()
{
    DFCar_Unload(_car);
    UnloadRenderTexture(_viewport);
    UnloadRenderTexture(_viewportQ1);
    UnloadRenderTexture(_viewportQ2);
    UnloadRenderTexture(_viewportQ3);
    UnloadRenderTexture(_viewportQ4);
}

static void _Run()
{
    if (_hasNextMode) {
        _mode = _nextMode;
        _hasNextMode = false;
        _nextMode = (CarEditorMode){0};
    }

    EditorCam_Update();
    if (IsKeyPressed(KEY_TAB)) {
        _quadView = !_quadView;
    }

    BeginDrawing();

    if (_quadView) {
        Camera3D c = {.fovy = 1, .projection = CAMERA_ORTHOGRAPHIC, .target = {0, 0, 0}};

        BeginTextureMode(_viewportQ1);
        ClearBackground(_bg3d);
        EditorCam_BeginRender();
        _mode.Render3D(_car);
        EditorCam_EndRender();
        EndTextureMode();

        c.position = (Vector3){0, 3, 0};
        c.up = (Vector3){1, 0, 0};
        BeginTextureMode(_viewportQ2);
        ClearBackground(_bg3d);
        BeginMode3D(c);
        _mode.Render3D(_car);
        EndMode3D();
        EndTextureMode();

        c.position = (Vector3){0, 0, -3};
        c.up = (Vector3){0, 1, 0};
        BeginTextureMode(_viewportQ3);
        ClearBackground(_bg3d);
        BeginMode3D(c);
        _mode.Render3D(_car);
        EndMode3D();
        EndTextureMode();

        c.position = (Vector3){-3, 0, 0};
        BeginTextureMode(_viewportQ4);
        ClearBackground(_bg3d);
        BeginMode3D(c);
        _mode.Render3D(_car);
        EndMode3D();
        EndTextureMode();
    } else {
        BeginTextureMode(_viewport);
        ClearBackground(_bg3d);
        EditorCam_BeginRender();
        _mode.Render3D(_car);
        EditorCam_EndRender();
        EndTextureMode();
    }

    if (_quadView) {
        ClearBackground(DARKGRAY);
        DrawTexturePro(
            _viewportQ1.texture,
            (Rectangle){.x = 0, .y = 0, .width = _viewportQ1.texture.width, .height = -_viewportQ1.texture.height},
            (Rectangle){.height = _viewportQ1.texture.height, .width = _viewportQ1.texture.width, .x = 640, .y = 0},
            (Vector2){0}, 0, WHITE);
        DrawText("3D", 642, 2, 8, RED);
        DrawTexturePro(
            _viewportQ2.texture,
            (Rectangle){.x = 0, .y = 0, .width = _viewportQ2.texture.width, .height = -_viewportQ2.texture.height},
            (Rectangle){.height = _viewportQ2.texture.height, .width = _viewportQ2.texture.width, .x = 1282, .y = 0},
            (Vector2){0}, 0, WHITE);
        DrawText("TOP", 1284, 2, 8, RED);
        DrawTexturePro(
            _viewportQ3.texture,
            (Rectangle){.x = 0, .y = 0, .width = _viewportQ3.texture.width, .height = -_viewportQ3.texture.height},
            (Rectangle){.height = _viewportQ3.texture.height, .width = _viewportQ3.texture.width, .x = 640, .y = 542},
            (Vector2){0}, 0, WHITE);
        DrawText("FRONT", 642, 544, 8, RED);
        DrawTexturePro(
            _viewportQ4.texture,
            (Rectangle){.x = 0, .y = 0, .width = _viewportQ4.texture.width, .height = -_viewportQ4.texture.height},
            (Rectangle){.height = _viewportQ4.texture.height, .width = _viewportQ4.texture.width, .x = 1282, .y = 542},
            (Vector2){0}, 0, WHITE);
        DrawText("SIDE", 1284, 544, 8, RED);
    } else {
        DrawTexturePro(
            _viewport.texture,
            (Rectangle){.x = 0, .y = 0, .width = _viewport.texture.width, .height = -_viewport.texture.height},
            (Rectangle){.height = 1080, .width = 1280, .x = 640, .y = 0},
            (Vector2){0}, 0, WHITE);
    }

    rlImGuiBegin();
    igSetNextWindowPos((ImVec2){0, 0}, ImGuiCond_Once, (ImVec2){0});
    igSetNextWindowSize((ImVec2){640, GetScreenHeight()}, ImGuiCond_Once);
    igBegin("Deformerz Car Editor", nullptr,
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    _mode.RenderUI(_car);
    igEnd();
    rlImGuiEnd();

    EndDrawing();
}

Vector2 CarEditor_GetMousePosViewport()
{
    Vector2 mp = GetMousePosition();
    return (Vector2){mp.x - 320, mp.y};
}
void CarEditor_SwitchMode(CarEditorMode mode)
{
    _nextMode = mode;
    _hasNextMode = true;
}
EngineContext CarEditorContext() { return (EngineContext){.OnEnter = _Enter, .OnExit = _Exit, .Run = _Run}; }
