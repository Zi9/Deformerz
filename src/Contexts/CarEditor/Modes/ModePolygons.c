#include "Contexts/CarEditor/CarEditor.h"
#include "Engine/DFCar.h"
#include "Engine/UI.h"
#include "LibTerep/TerepCar.h"

static int idx = 0;

static const char* _RenderData2Str(TerepCarRenderDataItem* item)
{
    switch (item->type) {
    case TEREP_RENDERDATA_NULL:
        return "NULL";
    case TEREP_RENDERDATA_CAMERA:
        return "CAMERADATA";
    case TEREP_RENDERDATA_UNK3_POLYGON:
        return "UNK3_POLYGON";
    case TEREP_RENDERDATA_COLOR_POLYGON:
        return "COLOR_POLYGON";
    case TEREP_RENDERDATA_TEXTURE_POLYGON:
        return "TEXTURE_POLYGON";
    case TEREP_RENDERDATA_WHEEL:
        return "WHEELDATA";
    }
}

static void _3D(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    for (int i = 0; i < car->renderDataCount; i++) {
        Color c = WHITE;
        if (i == idx) {
            c = GREEN;
        }
        if (car->renderData[i].type == TEREP_RENDERDATA_TEXTURE_POLYGON) {
            DFCar_RenderPolygonTextured(car->renderData[i].polygon, dfcar->carTex, c);
        } else if (car->renderData[i].type == TEREP_RENDERDATA_COLOR_POLYGON && car->renderData[i].polygon->closed) {
            DFCar_RenderPolygonColored(car->renderData[i].polygon, c);
        }
    }
}

static void _UI(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    if (igButton("<- Back", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_Meta());
    }
    igSameLine(0, 8);
    igText("Polygons");
    igSeparator();

    igPushID_Str("Polygons");
    igText("Hover: %i", idx);
    for (size_t i = 0; i < car->renderDataCount; i++) {
        if (car->renderData[i].type != TEREP_RENDERDATA_TEXTURE_POLYGON &&
            car->renderData[i].type != TEREP_RENDERDATA_COLOR_POLYGON)
            continue;
        igText("%i - %s (%i) Closed: %s", i, _RenderData2Str(&car->renderData[i]),
               car->renderData[i].polygon->vertexCount, car->renderData[i].polygon->closed ? "true" : "false");
        if (igIsItemHovered(0)) {
            idx = i;
        }
    }
    igSeparator();
    igPopID();
}

CarEditorMode CarEditorMode_Polygons() { return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D}; }
