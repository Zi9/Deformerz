#pragma once

#include "LibTerep/TerepMap.h"
#include <raylib.h>

typedef struct {
    TerepMap* map;
    Texture tex;
    Model model;
    Shader shader;
} DFMap;

DFMap* DFMap_Load();
void DFMap_Unload(DFMap* dfmap);
