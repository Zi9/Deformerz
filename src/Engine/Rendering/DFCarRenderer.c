#include "Engine/Core/DFCar.h"
#include <stdlib.h>

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
        DrawCube(car->points[i].pos, 0.05f, 0.05f, 0.05f, col);
        if (car->points[i].diameter > 0) {
            DrawCircle3D(car->points[i].pos, car->points[i].diameter, (Vector3){0.0f, 1.0f, 0.0f}, 90, PINK);
        }
    }
    for (size_t i = 0; i < car->physSegmentCount; i++) {
        if (car->currentSelSeg == i) {
            col = PURPLE;
        } else {
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
        DrawLine3D(car->points[car->physSegments[i].pointA].pos, car->points[car->physSegments[i].pointB].pos, col);
    }
}
