#include "Contexts/CarEditor/CarEditor.h"
#include "Engine/DFCar.h"
#include "Engine/UI.h"
#include "LibTerep/TerepCar.h"
#include "cimgui.h"
#include "raylib.h"

static BoundingBox _bbox = {0};
static bool _showbbox = false;

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
            DFCar_RenderPolygonTextured(car->renderData[i].polygon, dfcar->carTex, WHITE);
        } else if (car->renderData[i].type == TEREP_RENDERDATA_COLOR_POLYGON && car->renderData[i].polygon->closed) {
            DFCar_RenderPolygonColored(car->renderData[i].polygon, WHITE);
        }
    }
    if (_showbbox) {
        DrawBoundingBox(_bbox, GREEN);
    }
}

static void _UI(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    igSeparatorText("Quick Load Car");
    if (igButton("CAR1", (ImVec2){0})) {
        CarEditor_LoadCar("car1");
        return;
    }
    igSameLine(0, 8);
    if (igButton("CAR2", (ImVec2){0})) {
        CarEditor_LoadCar("car2");
        return;
    }
    igSameLine(0, 8);
    if (igButton("CAR3", (ImVec2){0})) {
        CarEditor_LoadCar("car3");
        return;
    }
    igSameLine(0, 8);
    if (igButton("CAR4", (ImVec2){0})) {
        CarEditor_LoadCar("car4");
        return;
    }
    igSameLine(0, 8);
    if (igButton("CAR5", (ImVec2){0})) {
        CarEditor_LoadCar("car5");
        return;
    }

    igSeparatorText("Currently Editing");
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

    if (igButton("Show Bounding Box", (ImVec2){0})) {
        _bbox.min = ToVector3(car->points[0].pos);
        _bbox.max = ToVector3(car->points[0].pos);
        for (int i = 1; i < car->pointCount; i++) {
            Vector3 p = ToVector3(car->points[i].pos);
            if (p.x < _bbox.min.x)
                _bbox.min.x = p.x;
            if (p.y < _bbox.min.y)
                _bbox.min.y = p.y;
            if (p.z < _bbox.min.z)
                _bbox.min.z = p.z;

            if (p.x > _bbox.max.x)
                _bbox.max.x = p.x;
            if (p.y > _bbox.max.y)
                _bbox.max.y = p.y;
            if (p.z > _bbox.max.z)
                _bbox.max.z = p.z;
        }
        _showbbox = true;
    }

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
