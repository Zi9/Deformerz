#include "Contexts/CarEditor/CarEditor.h"
#include "Engine/DFCar.h"
#include "Engine/UI.h"
#include "LibTerep/TerepCar.h"
#include <raylib.h>
#include <rlgl.h>

static int _idx = -1;

static void _3D(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    for (int i = 0; i < car->renderDataCount; i++) {
        if (car->renderData[i].type == TEREP_RENDERDATA_TEXTURE_POLYGON) {
            DFCar_RenderPolygonTextured(car->renderData[i].polygon, dfcar->carTex, ColorAlpha(WHITE, 0.5f));
        } else if (car->renderData[i].type == TEREP_RENDERDATA_COLOR_POLYGON && car->renderData[i].polygon->closed) {
            DFCar_RenderPolygonColored(car->renderData[i].polygon, ColorAlpha(WHITE, 0.5f));
        }
    }
    rlDrawRenderBatchActive();
    rlDisableDepthTest();
    // if (_idx == -1)
    //    return;
    Color c = RED;
    for (int i = 0; i < car->renderDataCount; i++) {
        if (car->renderData[i].type != TEREP_RENDERDATA_UNK3_POLYGON) continue;
        TerepCarPolygonData* poly = car->renderData[i].polygon;
        if (i > 39)
            c = GREEN;
        if (i > 50)
            c = BLUE;
        DrawLine3D(ToVector3(poly->vertices[0]->pos), ToVector3(poly->vertices[1]->pos), c);
        DrawLine3D(ToVector3(poly->vertices[1]->pos), ToVector3(poly->vertices[2]->pos), c);
    }
    rlDrawRenderBatchActive();
    rlEnableDepthTest();
}

static void _UI(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    if (igButton("<- Back", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_Meta());
    }
    igSameLine(0, 8);
    igText("Type 3 Polygons");
    igSeparator();

    igText("Hovered: %i", _idx);

    igSeparator();
    igPushID_Str("Type3");
    for (size_t i = 0; i < car->renderDataCount; i++) {
        if (car->renderData[i].type != TEREP_RENDERDATA_UNK3_POLYGON)
            continue;
        TerepCarPolygonData* p = car->renderData[i].polygon;
        igText("%i - %i, %i, %i", i, p->unknown3values[0], p->unknown3values[1], p->unknown3values[2]);
        if (igIsItemHovered(0)) {
            _idx = i;
        }
    }
    igSeparator();
    igPopID();
}

CarEditorMode CarEditorMode_Unk3() { return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D}; }
