#include "Engine.h"
#include "Assets/AssetManager.h"
#include "Engine/Rendering/DFCamera.h"
#include "Rendering/Renderer.h"
#include <raylib.h>

EngineData Engine = {0};

void Engine_Initialize()
{
    SetTraceLogLevel(LOG_ERROR);
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    Renderer_Initialize();
    Engine.car = Assets_LoadDFCar("data/terep/car1.dat");
    Engine.map = Assets_LoadDFMap(Engine.cfg.baseDataPath);
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
    Assets_UnloadDFCar(Engine.car);
    Assets_UnloadDFMap(Engine.map);
}

Config Engine_Main(Config cfg)
{
    Engine.cfg = cfg;
    Engine_Initialize();
    while (!WindowShouldClose()) {
        Engine_Loop();
    }
    Engine_Destroy();
    return Engine.cfg;
}

/*
struct GameState {
    DFMap* currentMap;
    DFCar* currentCar;
    struct {
        bool wireframe;
    } render;
} game = {0};

void game_load(Config cfg)
{
    game.activeCam = camera_create();
    game.currentMap = map_load(cfg.baseDataPath);
    game.currentCar = car_load("./data/terep/car2.dat");
    game.currentCar->currentSelSeg = 0;
    game.render.wireframe = false;
}

void game_unload()
{
    if (game.currentMap != NULL) {
        map_unload(game.currentMap);
        game.currentMap = NULL;
    }
    if (game.currentCar != NULL) {
        car_unload(game.currentCar);
        game.currentCar = NULL;
    }
}

void game_render()
{
    if (game.currentMap != NULL)
        map_render(game.currentMap);
    if (game.currentCar != NULL)
        car_render(game.currentCar);
}

Config game_main(Config initialConfig)
{
    Config cfg = initialConfig;
    InitWindow(cfg.render.width * cfg.render.upscaleMultiplier, cfg.render.height * cfg.render.upscaleMultiplier,
               GAME_WINDOW_TITLE);
    SetTargetFPS(60);
    //SetExitKey(KEY_CAPS_LOCK);

    RenderTexture2D target;
    if (cfg.render.upscaleMultiplier == 1) {
        target = LoadRenderTexture(cfg.render.width * cfg.render.upscaleMultiplier,
                                   cfg.render.height * cfg.render.upscaleMultiplier);
    } else {
        target = LoadRenderTexture(cfg.render.width, cfg.render.height);
    }

    game_load(cfg);

    while (!WindowShouldClose()) {
        camera_update(&game.activeCam, GetFrameTime());

        // if (IsKeyPressed(KEY_F1))
        //     cfg.restart = false;

        if (IsKeyPressed(KEY_F7))
            game.render.wireframe = !game.render.wireframe;
        if (IsKeyPressed(KEY_RIGHT))
            game.currentCar->currentSelSeg++;
        if (IsKeyPressed(KEY_LEFT))
            game.currentCar->currentSelSeg--;
        // if (IsKeyPressed(KEY_F8))
        game.currentMap->model.materials[0].shader = game.currentMap->affineShd;
        // if (IsKeyPressed(KEY_F9))
        //     map->model.materials[0].shader = map->normalShd;

        BeginDrawing();
        BeginTextureMode(target);
        ClearBackground(cfg.skyColor);

        BeginMode3D(game.activeCam.rlCam);

        if (game.render.wireframe)
            rlEnableWireMode();
        game_render();
        if (game.render.wireframe)
            rlDisableWireMode();

        EndMode3D();

        EndTextureMode();

        DrawTexturePro(target.texture, (Rectangle){0, 0, target.texture.width, -target.texture.height},
                       (Rectangle){0, 0, cfg.render.width * cfg.render.upscaleMultiplier,
                                   cfg.render.height * cfg.render.upscaleMultiplier},
                       (Vector2){0, 0}, 0.0f, WHITE);
        DrawText("Deformers - Development Build", 1, 1, 20, BLACK);
        DrawText("Deformers - Development Build", 0, 0, 20, RED);
        EndDrawing();
    }
    game_unload();
    CloseWindow();
    return cfg;
}
*/
