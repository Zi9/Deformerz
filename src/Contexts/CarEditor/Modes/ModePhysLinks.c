#include "Contexts/CarEditor/CarEditor.h"
#include "Engine/DFCar.h"
#include "Engine/UI.h"
#include "LibTerep/TerepCar.h"
#include "cimgui.h"
#include "raylib.h"
#include <raymath.h>

static void _RecalculatePhysLinks(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    for (int j = 0; j < car->physLinkCount; j++) {
        TerepCarPhysLink* p = &car->physLinks[j];
        p->len = Vector3Distance(ToVector3(p->pointA->pos), ToVector3(car->physLinks[j].pointB->pos));
        p->len2 = car->physLinks[j].len;
        if (p->type == TEREP_PHYSLINK_SUSP_FRONT10) {
            p->len_min = car->physLinks[j].len * 0.48;
            p->len_max = car->physLinks[j].len;
        } else if (p->type == TEREP_PHYSLINK_SUSP_FRONT12) {
            p->len_min = car->physLinks[j].len * 0.48;
            p->len_max = car->physLinks[j].len;
        } else if (p->type == TEREP_PHYSLINK_SUSP_REAR4) {
            p->len_min = car->physLinks[j].len * 0.45;
            p->len_max = car->physLinks[j].len;
        } else if (p->type == TEREP_PHYSLINK_SUSP_REAR6) {
            p->len_min = car->physLinks[j].len * 0.45;
            p->len_max = car->physLinks[j].len;
        } else if (p->type == TEREP_PHYSLINK_SUSP_EXTRA) {
            p->len_min = car->physLinks[j].len * 0.45;
            p->len_max = car->physLinks[j].len * 1.05;
        } else {
            p->len_min = car->physLinks[j].len * 0.5;
            p->len_max = car->physLinks[j].len * 1.5;
        }
    }
}

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
    for (size_t i = 0; i < car->physLinkCount; i++) {
        TerepCarPhysLink* p = &car->physLinks[i];
        DrawLine3D(ToVector3(p->pointA->pos), ToVector3(p->pointB->pos), WHITE);
    }
}

static void _UI(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    if (igButton("<- Back", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_Meta());
    }
    igSameLine(0, 8);
    igText("Physics Links");
    igSeparator();

    igPushID_Str("PhysSeg");
    if (igButton("Recalculate All Physics Links", (ImVec2){0})) {
        _RecalculatePhysLinks(dfcar);
    }
    for (size_t i = 0; i < car->physLinkCount; i++) {
        TerepCarPhysLink* p = &car->physLinks[i];
        igText("%i - %s = %i <-> %i | Min: %f (%f%) Max: %f (%f%) Val:%f/%f", i, TerepCar_PhysLink2String(p),
               p->pointA->index, p->pointB->index, p->len_min, 100.0f / p->len * p->len_min, p->len_max,
               100.0f / p->len * p->len_max, p->len, p->len2);
    }
    igSeparator();
    igPopID();
}

CarEditorMode CarEditorMode_PhysLinks() { return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D}; }
