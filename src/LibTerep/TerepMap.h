#pragma once

#include "PCX.h"
#include <stdint.h>

typedef struct {
    // Mesh
    int vertexCount;
    int triangleCount;
    float* vertices;
    float* uvs;

    // Other data
    uint8_t* heightmap;
    uint8_t* colormap;

    PCXImage* texturemap;

    // Palette
    struct RGBColor palette[256];
} TerepMap;

TerepMap* TerepMap_Load(const char* colpcx, const char* mappcx, const char* maptexpcx);
void TerepMap_Unload(TerepMap* map);
