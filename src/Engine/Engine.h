#pragma once

#include "Config.h"
#include "Core/DFCar.h"
#include "Core/DFMap.h"

typedef struct EngineData {
    float dt;
    float time;
    Config cfg;

    DFCar* car;
    DFMap* map;
    Color skyColor;
} EngineData;

extern EngineData Engine;

Config Engine_Main(Config cfg);
