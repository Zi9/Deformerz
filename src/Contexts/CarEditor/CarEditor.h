#pragma once

#include "Engine/DFCar.h"
#include "Engine/Engine.h"
#include "LibTerep/TerepCar.h"
#include "raylib.h"

typedef struct {
    void (*RenderUI)(DFCar*);
    void (*Render3D)(DFCar*);
} CarEditorMode;

void CarEditor_SwitchMode(CarEditorMode mode);
Vector2 CarEditor_GetMousePosViewport();

CarEditorMode CarEditorMode_Meta();
CarEditorMode CarEditorMode_Points();
CarEditorMode CarEditorMode_PhysLinks();
CarEditorMode CarEditorMode_Polygons();
CarEditorMode CarEditorMode_WheelData();
CarEditorMode CarEditorMode_Renderables();
CarEditorMode CarEditorMode_Unk3();
CarEditorMode CarEditorMode_EditPoint(TerepCarPoint* point);
CarEditorMode CarEditorMode_EditPhyslink(TerepCarPhysLink* link);
CarEditorMode CarEditorMode_EditPolygon(TerepCarPolygonData* poly);

EngineContext CarEditorContext();
