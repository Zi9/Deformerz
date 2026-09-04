#include "Engine/DFCar.h"
#include "Contexts/CarEditor/CarEditor.h"
#include "Engine/UI.h"
#include "LibTerep/TerepCar.h"
#include "cimgui.h"

static void _UI_U16Editor(const char* label, uint16_t* value)
{
    int v = *value;
    igInputInt(label, &v, 1, 100, 0);
    v = v < 0 ? 0 : v > UINT16_MAX ? UINT16_MAX : v;
    *value = v;
}

static void _3D(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    for (int i = 0; i < car->pointCount; i++) {
        switch (car->points[i].type) {
        case TEREP_POINT_GEOMETRY:
            break;
        case TEREP_POINT_CAMERA:
            DFCar_RenderPoint(&car->points[i], MAGENTA);
            break;
        case TEREP_POINT_WHEEL_FRONT:
            DFCar_RenderPoint(&car->points[i], RED);
            break;
        case TEREP_POINT_WHEEL_REAR:
            DFCar_RenderPoint(&car->points[i], BLUE);
            break;
        }
    }
    for (int i = 0; i < car->renderDataCount; i++) {
        if (car->renderData[i].type == TEREP_RENDERDATA_TEXTURE_POLYGON) {
            DFCar_RenderPolygonTextured(car->renderData[i].polygon, dfcar->carTex, ColorAlpha(WHITE, 0.5f));
        } else if (car->renderData[i].type == TEREP_RENDERDATA_COLOR_POLYGON && car->renderData[i].polygon->closed) {
            DFCar_RenderPolygonColored(car->renderData[i].polygon, ColorAlpha(WHITE, 0.5f));
        }
    }
}

static void _UI(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    igInputText("DAT file", dfcar->name, 128, 0, 0, 0);
    igSameLine(0, 8);
    if (igButton("Save", (ImVec2){0})) {
        TerepCar_Write(dfcar->car, dfcar->name, 0);
    }
    igSameLine(0, 8);
    if (igButton("Save as Text", (ImVec2){0})) {
        TerepCar_WriteText(dfcar->car, "./data/car1.txt");
    }

    igText("Points: %i", car->pointCount);
    igText("Physics Links: %i", car->physLinkCount);
    igText("Render Data Items: %i", car->renderDataCount);

    igSeparatorText("Switch Editor Mode");
    if (igButton("Points", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_Points());
    }
    igSameLine(0, 8);
    if (igButton("PhysLinks", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_PhysLinks());
    }
    igSameLine(0, 8);
    if (igButton("Polygons", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_Polygons());
    }
    igSameLine(0, 8);
    if (igButton("Wheels", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_WheelData());
    }
    igSameLine(0, 8);
    if (igButton("All Renderables", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_Renderables());
    }
    igSameLine(0, 8);
    if (igButton("Type3", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_Unk3());
    }

    igSeparatorText("Header Data");
    _UI_U16Editor("Header Value 1", &car->unknownHeaderValue1);
    _UI_U16Editor("Engine Sound", &car->engineSound);
}

CarEditorMode CarEditorMode_Meta() { return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D}; }
