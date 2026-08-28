#include "CarEditor.h"
#include "Engine/Rendering/DFCamera.h"
#include "Engine/Rendering/Renderer.h"
#include "LibTerep/TerepCar.h"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

void CarEditor_Render3D(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    switch (CarEditor.Mode) {
    case MODE_DEFAULT:
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
            Ray ray = GetScreenToWorldRay(GetMousePosition(), DFCamera_GetRLCamera());
            RayCollision rc = GetRayCollisionBox(
                ray, (BoundingBox){.min = Vector3SubtractValue(pos, 0.01f), .max = Vector3AddValue(pos, 0.01f)});
            col.a = rc.hit ? 255 : 128;
            Render_CarPoint(&car->points[i], col);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && rc.hit && CarEditor.HandleMouseIn3D) {
                CarEditor.SelectedPoint = i;
                CarEditor.Mode = MODE_EDIT_POINT;
            }
        }
        break;

    case MODE_EDIT_POINT:
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CarEditor.HandleMouseIn3D) {
            CarEditor.Mode = MODE_DEFAULT;
        }
        Render_CarPoint(&car->points[CarEditor.SelectedPoint], GREEN);
        for (int j = 0; j < car->physSegmentCount; j++) {
            if (car->physSegments[j].pointA == CarEditor.SelectedPoint ||
                car->physSegments[j].pointB == CarEditor.SelectedPoint) {
                Render_CarPhysicsSegment(&car->physSegments[j], car->points, WHITE);
            }
        }
    case MODE_EDIT_PHYSLINK:
        break;
    case MODE_EDIT_UV:
        break;
    }
    for (int i = 0; i < car->renderDataCount; i++) {
        if (car->renderData[i].type == TEREP_RENDERDATA_TEXTURE_POLYGON) {
            Render_CarPolygonTextured(car->renderData[i].polygon, car->points, dfcar->carTex, ColorAlpha(WHITE, 0.5f));
        } else if (car->renderData[i].type == TEREP_RENDERDATA_COLOR_POLYGON && car->renderData[i].polygon->closed) {
            Render_CarPolygonColored(car->renderData[i].polygon, car->points, ColorAlpha(WHITE, 0.5f));
        }
    }
}
