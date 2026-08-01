#pragma once

#include "Engine/Core/DFCar.h"
#include "Engine/Core/DFMap.h"

void Renderer_Initialize();
void Renderer_Destroy();

void Renderer_Render();

void Renderer_RenderDFCar(DFCar* car);
void Renderer_RenderDFMap(DFMap* map);
