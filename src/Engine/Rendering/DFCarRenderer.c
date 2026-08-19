#include "Engine/Core/DFCar.h"
#include "Engine/Engine.h"
#include "Engine/UI/ImGUI.h"
#include "cimgui.h"
#include <raylib.h>
#include <stdlib.h>

void Renderer_RenderDFCar(DFCar* car)
{
    Color col;
    for (size_t i = 0; i < car->pointCount; i++) {
        switch (car->points[i].type) {
        case DFCAR_POINT_GEOMETRY:
            col = WHITE;
            break;
        case DFCAR_POINT_CAMERA:
            col = MAGENTA;
            break;
        case DFCAR_POINT_WHEEL_FL:
        case DFCAR_POINT_WHEEL_FR:
        case DFCAR_POINT_WHEEL_RL:
        case DFCAR_POINT_WHEEL_RR:
            col = BLUE;
            break;
        }
        DrawCube(car->points[i].pos, 0.02f, 0.02f, 0.02f, col);
        if (car->points[i].size > 0) {
            if (car->points[i].type == DFCAR_POINT_CAMERA) {
                DrawSphere(car->points[i].pos, car->points[i].size, PINK);
            } else {
                DrawCircle3D(car->points[i].pos, car->points[i].size, (Vector3){0.0f, 1.0f, 0.0f}, 90, PINK);
            }
        }
    }
    for (size_t i = 0; i < car->physSegmentCount; i++) {
        switch (car->physSegments[i].type) {
        case DFCAR_SEGMENT_NORMAL:
            col = WHITE;
            break;
        case DFCAR_SEGMENT_SUSP_FRONT:
            col = BLUE;
            break;
        case DFCAR_SEGMENT_SUSP_REAR:
            col = RED;
            break;
        case DFCAR_SEGMENT_SUSP_EXTRA:
            col = GREEN;
            break;
        }
    }
    for (size_t i = 0; i < car->renderableFaceCount; i++) {
        if (car->renderableFaces[i].render == false)
            continue;
        if (car->renderableFaces[i].colors[0] == 255)
            continue;
        DrawTriangle3D(car->points[car->renderableFaces[i].vertices[2]].pos, car->points[car->renderableFaces[i].vertices[1]].pos, car->points[car->renderableFaces[i].vertices[0]].pos, Engine.palette[car->renderableFaces[i].colors[0]]);
        if (car->renderableFaces[i].count == 4) {
        DrawTriangle3D(car->points[car->renderableFaces[i].vertices[0]].pos, car->points[car->renderableFaces[i].vertices[3]].pos, car->points[car->renderableFaces[i].vertices[2]].pos, Engine.palette[car->renderableFaces[i].colors[0]]);
        }
    }
}
bool debugActive = true;
void Renderer_DFCarDebugger(DFCar* car)
{
    igSetNextWindowPos((ImVec2){32, 32}, ImGuiCond_Once, (ImVec2){0});
    igBegin("Car Debug", &debugActive, ImGuiWindowFlags_NoCollapse);
    igText("Renderables");
    igPushID_Str("Renderables");
    for (size_t i = 0; i < car->renderableFaceCount; i++) {
        char* label = TextFormat("%i - c:%i", i, car->renderableFaces[i].colors[0]);
        igCheckbox(label, &car->renderableFaces[i].render);
    }
    igPopID();
    igEnd();
}
