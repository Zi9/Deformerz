#pragma once

#include "PCX.h"
#include <stdint.h>

typedef struct {
    int vertexCount;
    int triangleCount;
    float* vertices;
    float* uvs;
    PCXData* heightmap;
    PCXData* colormap;
    PCXImage* texturemap;
} TerepMap;

TerepMap* TerepMap_Load(const char* colpcx, const char* mappcx, const char* maptexpcx);
void TerepMap_Unload(TerepMap* map);
