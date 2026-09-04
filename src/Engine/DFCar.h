#pragma once

#include "LibTerep/TerepCar.h"
#include <raylib.h>

typedef struct {
    TerepCar* car;
    Texture2D carTex;
    char name[128];
} DFCar;

DFCar* DFCar_Load();
void DFCar_Unload(DFCar* dfcar);

#define ToVector3(v)                                                                                                   \
    (Vector3) { v[0], v[1], v[2] }

void DFCar_RenderPoint(TerepCarPoint* point, Color tint);
void DFCar_RenderPhysicsLink(TerepCarPhysLink* seg, Color tint);
void DFCar_RenderPolygonColored(TerepCarPolygonData* face, Color tint);
void DFCar_RenderPolygonTextured(TerepCarPolygonData* face, Texture tex, Color tint);
