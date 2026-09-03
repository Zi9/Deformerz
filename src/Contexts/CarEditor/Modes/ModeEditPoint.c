#include "Engine/DFCar.h"
#include "Contexts/CarEditor/CarEditor.h"
#include "Engine/UI.h"
#include "LibTerep/TerepCar.h"
#include "raymath.h"
#include <raylib.h>

static TerepCarPoint* _p;

static const char* _Point2Str(TerepCarPoint* point)
{
    switch (point->type) {
    case TEREP_POINT_GEOMETRY:
        return "GEOMETRY";
    case TEREP_POINT_CAMERA:
        return "CAMERA";
    case TEREP_POINT_WHEEL_FRONT:
        return "WHEEL_F";
    case TEREP_POINT_WHEEL_REAR:
        return "WHEEL_R";
    }
}

static void _RecalculatePhysLinks(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    for (int j = 0; j < car->physLinkCount; j++) {
        if (car->physLinks[j].pointA == _p || car->physLinks[j].pointB == _p) {
            car->physLinks[j].len = Vector3Distance(ToVector3(car->physLinks[j].pointA->pos), ToVector3(car->physLinks[j].pointB->pos));
            car->physLinks[j].len2 = car->physLinks[j].len;
            car->physLinks[j].len_min = car->physLinks[j].len * 0.5;
            car->physLinks[j].len_max = car->physLinks[j].len * 1.5;
        }
    }
}

static void _3D(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    for (int i = 0; i < car->pointCount; i++) {
        if (&car->points[i] == _p) {
            DFCar_RenderPoint(_p, GREEN);
            for (int j = 0; j < car->physLinkCount; j++) {
                if (car->physLinks[j].pointA == _p || car->physLinks[j].pointB == _p) {
                    DFCar_RenderPhysicsLink(&car->physLinks[j], WHITE);
                }
            }
        } else {
            Vector3 pos = ToVector3(car->points[i].pos);
            DrawCube(pos, 0.01f, 0.01f, 0.01f, ColorAlpha(WHITE, 0.25f));
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
    if (igButton("<- Back", (ImVec2){0})) {
        _RecalculatePhysLinks(dfcar);
        CarEditor_SwitchMode(CarEditorMode_Points());
    }
    igSameLine(0, 8);
    igText("Editing Point: %i (%s)", _p->index, _Point2Str(_p));
    igSeparator();

    igSliderFloat3("Position", _p->pos, -1, 1, "", 0);
    igText("%f, %f, %f S:%f", _p->pos[0], _p->pos[1], _p->pos[2], _p->size);
}

CarEditorMode CarEditorMode_EditPoint(TerepCarPoint* point)
{
    _p = point;
    return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D};
}
