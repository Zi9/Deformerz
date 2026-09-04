#include "Engine/DFCar.h"
#include "Contexts/CarEditor/CarEditor.h"
#include "Contexts/CarEditor/EditorCam.h"
#include "Engine/UI.h"
#include "LibTerep/TerepCar.h"
#include <raymath.h>

static bool mouse3D = false;

static void _3D(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;

    for (int i = 0; i < car->pointCount; i++) {
        Color col;
        switch (car->points[i].type) {
        case TEREP_POINT_GEOMETRY:
            col = WHITE;
            break;
        case TEREP_POINT_CAMERA:
            col = MAGENTA;
            break;
        case TEREP_POINT_WHEEL_FRONT:
            col = RED;
            break;
        case TEREP_POINT_WHEEL_REAR:
            col = BLUE;
            break;
        }
        Vector3 pos = ToVector3(car->points[i].pos);
        Ray ray = GetScreenToWorldRay(CarEditor_GetMousePosViewport(), EditorCam_GetRLCamera());
        RayCollision rc = GetRayCollisionBox(
            ray, (BoundingBox){.min = Vector3SubtractValue(pos, 0.01f), .max = Vector3AddValue(pos, 0.01f)});
        col.a = rc.hit ? 255 : 128;
        DFCar_RenderPoint(&car->points[i], col);
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && rc.hit && mouse3D) {
            CarEditor_SwitchMode(CarEditorMode_EditPoint(&car->points[i]));
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
    if (igButton("<- Back", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_Meta());
    }
    igSameLine(0, 8);
    igText("Selecting point");
    igSeparator();
    igPushID_Str("Points");
    for (size_t i = 0; i < car->pointCount; i++) {
        igText("%i - %s (%f, %f, %f) S:%f", i, TerepCar_Point2String(&car->points[i]), car->points[i].pos[0],
               car->points[i].pos[1], car->points[i].pos[2], car->points[i].size);
    }
    igPopID();
    mouse3D = !igGetIO_Nil()->WantCaptureMouse;
}

CarEditorMode CarEditorMode_Points() { return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D}; }
