#pragma once

#include "Engine/Core/DFCar.h"
#include "Engine/Core/DFMap.h"
#include <raylib.h>

#define GAME_WINDOW_TITLE "Deformerz - A recreation of Terep2"
#define WMARK "Deformerz - v0.1"

typedef struct {
    float dt;
    float time;

    DFCar* car;
    DFMap* map;

    Color skyColor;
    Color palette[256];
} EngineData;

extern EngineData Engine;

void Engine_Main();
