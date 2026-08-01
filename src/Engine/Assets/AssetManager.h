#pragma once

#include "Engine/Core/DFCar.h"
#include "Engine/Core/DFMap.h"

DFCar* Assets_LoadDFCar(const char* path);
void Assets_UnloadDFCar(DFCar* car);

DFMap* Assets_LoadDFMap(const char* basePath);
void Assets_UnloadDFMap(DFMap* map);
