#pragma once

#include "Engine/Core/DFCar.h"
#include "Engine/Core/DFMap.h"

extern Color Renderer_Palette[256];
extern Color Renderer_SkyColor;

void Renderer_Initialize();
void Renderer_Destroy();
void Renderer_Render();

void Render_DFCar(DFCar* car);
void Render_DFMap(DFMap* map);

#define ToVector3(v) (Vector3){v[0], v[1], v[2]}

void Render_CarPoint(TerepCarPoint* point, Color tint);
void Render_CarPhysicsSegment(TerepCarPhysSegment* seg, TerepCarPoint* points, Color tint);
void Render_CarPolygonColored(TerepCarPolygonData* face, TerepCarPoint* points, Color tint);
void Render_CarPolygonTextured(TerepCarPolygonData* face, TerepCarPoint* points, Texture tex, Color tint);

//void Renderer_DFCarDebugger(DFCar* car);
