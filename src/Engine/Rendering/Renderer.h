#pragma once

#include "LibTerep/TerepCar.h"
//#include "Engine/Core/DFMap.h"

void Renderer_Initialize();
void Renderer_Destroy();

void Renderer_Render();

void Renderer_RenderCar(TerepCar* car);
//void Renderer_RenderDFMap(DFMap* map);


//void Renderer_DFCarDebugger(DFCar* car);
