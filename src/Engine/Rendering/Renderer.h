#pragma once

#include "Engine/Core/DFCar.h"
#include "Engine/Core/DFMap.h"

void Renderer_Initialize();
void Renderer_Destroy();

void Renderer_Render();

void Renderer_RenderCar(DFCar* car);
void Renderer_RenderMap(DFMap* map);


//void Renderer_DFCarDebugger(DFCar* car);
