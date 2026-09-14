#include "Contexts/CarEditor/CarEditor.h"
#include "Contexts/CarEditor/EditorCam.h"
#include "Engine/DFCar.h"
#include "Engine/UI.h"
#include "LibTerep/TerepCar.h"
#include <raymath.h>

static bool _mouse3D = false;
static bool _showPhys = false;
static bool _editMirrored = false;
static TerepCarPoint* _p;
static TerepCarPoint* _pm;

#define MIN_MIRROR_DIST 0.001f

static void _FindMirrorPoint(TerepCar* car)
{
    Vector3 mirrored = ToVector3(_p->pos);
    mirrored.x = -mirrored.x;
    for (int i = 0; i < car->pointCount; i++) {
        float curdistance = Vector3Distance(ToVector3(car->points[i].pos), mirrored);
        if (curdistance < MIN_MIRROR_DIST) {
            _pm = &car->points[i];
            return;
        }
    }
}

static void _3D(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;

    if (_p) {
        for (int i = 0; i < car->pointCount; i++) {
            if (&car->points[i] == _p) {
                DFCar_RenderPoint(_p, GREEN);
                if (_showPhys) {
                    for (int j = 0; j < car->physLinkCount; j++) {
                        if (car->physLinks[j].pointA == _p || car->physLinks[j].pointB == _p) {
                            DFCar_RenderPhysicsLink(&car->physLinks[j], WHITE);
                        }
                    }
                }
            } else if (&car->points[i] == _pm && _editMirrored) {
                DFCar_RenderPoint(_pm, BLUE);
            } else {
                Vector3 pos = ToVector3(car->points[i].pos);
                DrawCube(pos, 0.01f, 0.01f, 0.01f, ColorAlpha(WHITE, 0.25f));
            }
            Vector3 pos = ToVector3(car->points[i].pos);
            Ray ray = GetScreenToWorldRay(CarEditor_GetMousePosViewport(), EditorCam_GetRLCamera());
            RayCollision rc = GetRayCollisionBox(
                ray, (BoundingBox){.min = Vector3SubtractValue(pos, 0.01f), .max = Vector3AddValue(pos, 0.01f)});
            if (rc.hit) {
                Color col = GREEN;
                DFCar_RenderPoint(&car->points[i], col);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && _mouse3D) {
                    _p = &car->points[i];
                    _FindMirrorPoint(car);
                }
            }
        }
    } else {
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
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && rc.hit && _mouse3D) {
                _p = &car->points[i];
                _FindMirrorPoint(car);
            }
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
        if (_p) {
            _p = NULL;
        } else {
            CarEditor_SwitchMode(CarEditorMode_Meta());
        }
    }
    igSameLine(0, 8);
    igText("Points");
    igSeparator();

    if (_p) {
        igText("Editing Point: %i (%s)", _p->index, TerepCar_Point2String(_p));

        bool modified = igSliderFloat3("Position", _p->pos, -1, 1, "", 0);
        if (_editMirrored && modified)
        {
            _pm->pos[0] = -_p->pos[0];
            _pm->pos[1] = _p->pos[1];
            _pm->pos[2] = _p->pos[2];
        }
        igText("%f, %f, %f S:%f", _p->pos[0], _p->pos[1], _p->pos[2], _p->size);

        igSeparator();

        igCheckbox("Edit Symmetrically", &_editMirrored);
        igSameLine(0, 8);
        igCheckbox("Show Physics PhysLinks", &_showPhys);

        if (igButton("Apply and recalculate physics", (ImVec2){-1, 0})) {
            TerepCar_RecalculateAllPhysLinks(dfcar->car);
            _p = NULL;
            return;
        }
    } else {
        igPushID_Str("Points");
        for (size_t i = 0; i < car->pointCount; i++) {
            igText("%i - %s (%f, %f, %f) S:%f", i, TerepCar_Point2String(&car->points[i]), car->points[i].pos[0],
                   car->points[i].pos[1], car->points[i].pos[2], car->points[i].size);
        }
        igPopID();
    }
    _mouse3D = !igGetIO_Nil()->WantCaptureMouse;
}

CarEditorMode CarEditorMode_Points() { return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D}; }
