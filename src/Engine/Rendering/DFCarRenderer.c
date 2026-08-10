#include "Engine/Core/DFCar.h"
#include "raylib.h"
#include <stdlib.h>

void DrawPhysSeg(DFCar* car, int idx)
{
    DrawLine3D(car->points[car->physSegments[idx].pointA].pos, car->points[car->physSegments[idx].pointB].pos, RED);
}

void Renderer_RenderDFCar(DFCar* car)
{
    Color col;
    for (size_t i = 0; i < car->pointCount; i++) {
        switch (car->points[i].type) {
        case DFCAR_POINT_GEOMETRY:
            col = BLACK;
            break;
        case DFCAR_POINT_CAMERA:
            col = MAGENTA;
            break;
        case DFCAR_POINT_WHEEL_FL:
        case DFCAR_POINT_WHEEL_FR:
            col = BLUE;
            break;
        case DFCAR_POINT_WHEEL_RL:
        case DFCAR_POINT_WHEEL_RR:
            col = RED;
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
        // DrawLine3D(car->points[car->physSegments[i].pointA].pos, car->points[car->physSegments[i].pointB].pos, col);
        DrawPhysSeg(car, 43);
        DrawPhysSeg(car, 33);
        DrawPhysSeg(car, 21);
        DrawPhysSeg(car, 39);
        DrawPhysSeg(car, 43);
    }
}
