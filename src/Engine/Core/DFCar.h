#pragma once

#include "LibTerep/TerepCar.h"
#include <raylib.h>

typedef struct {
    TerepCar* car;
    Texture2D carTex;
} DFCar;

DFCar* DFCar_Load();
void DFCar_Unload(DFCar* dfcar);
