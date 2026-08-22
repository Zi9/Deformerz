#pragma once

#include "LibTerep/TerepCar.h"
#include "LibTerep/TerepMap.h"
#include <raylib.h>

#define GAME_WINDOW_TITLE "Deformerz - A recreation of Terep2"
#define WMARK "Deformerz - v0.1"

typedef struct EngineData {
    float dt;
    float time;

    TerepCar* car;
    TerepMap* map;
    Color skyColor;
    Color palette[256];
} EngineData;

extern EngineData Engine;

void Engine_Main();
