#include "Engine/Engine.h"
#include "LibTerep/TerepCar.h"
#include <raylib.h>
#include <rlgl.h>
#include <stdlib.h>
#include <string.h>

static inline Vector3 ToVector3(float v[3]) { return (Vector3){v[0], v[1], v[2]}; }

static void RenderPoint(TerepCarPoint* point)
{
    Color col;
    switch (point->type) {
    case TEREP_POINT_GEOMETRY:
        col = WHITE;
        break;
    case TEREP_POINT_CAMERA:
        col = MAGENTA;
        break;
    case TEREP_POINT_WHEEL_FRONT:
    case TEREP_POINT_WHEEL_REAR:
        col = BLUE;
        break;
    }
    Vector3 pos = ToVector3(point->pos);
    DrawCube(pos, 0.02f, 0.02f, 0.02f, col);
    if (point->size > 0) {
        if (point->type == TEREP_POINT_CAMERA) {
            DrawSphere(pos, point->size, PINK);
        } else {
            DrawCircle3D(pos, point->size, (Vector3){0.0f, 1.0f, 0.0f}, 90, PINK);
        }
    }
}

static void RenderPhysicsSegment(TerepCarPhysSegment* seg, TerepCarPoint* points)
{
    Color col;
    switch (seg->type) {
    case TEREP_SEGMENT_NORMAL:
        col = WHITE;
        break;
    case TEREP_SEGMENT_SUSP_FRONT:
    case TEREP_SEGMENT_SUSP_FRONT2:
        col = BLUE;
        break;
    case TEREP_SEGMENT_SUSP_REAR:
    case TEREP_SEGMENT_SUSP_REAR2:
        col = RED;
        break;
    case TEREP_SEGMENT_SUSP_EXTRA:
        col = GREEN;
        break;
    }
    DrawLine3D(ToVector3(points[seg->pointA].pos), ToVector3(points[seg->pointB].pos), col);
}

static void RenderPolygon(TerepCarPolygon* face, TerepCarPoint* points)
{
    Color color = WHITE;
    rlBegin(RL_TRIANGLES);
    if (face->type == TEREP_POLYGON_COLOR)
        color = Engine.palette[face->colors[0]];
    rlColor4ub(color.r, color.g, color.b, color.a);

    switch (face->pointCount) {
    case 3:
        color = RED;
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        rlVertex3f(points[face->vertices[1]].pos[0], points[face->vertices[1]].pos[1],
                   points[face->vertices[1]].pos[2]);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        break;
    case 4:
        color = RED;
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        rlVertex3f(points[face->vertices[1]].pos[0], points[face->vertices[1]].pos[1],
                   points[face->vertices[1]].pos[2]);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        color = GREEN;
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        rlVertex3f(points[face->vertices[3]].pos[0], points[face->vertices[3]].pos[1],
                   points[face->vertices[3]].pos[2]);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        break;
    case 5:
        color = RED;
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        rlVertex3f(points[face->vertices[1]].pos[0], points[face->vertices[1]].pos[1],
                   points[face->vertices[1]].pos[2]);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        color = GREEN;
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        rlVertex3f(points[face->vertices[4]].pos[0], points[face->vertices[4]].pos[1],
                   points[face->vertices[4]].pos[2]);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        color = BLUE;
        rlColor4ub(color.r, color.g, color.b, color.a);
        rlVertex3f(points[face->vertices[4]].pos[0], points[face->vertices[4]].pos[1],
                   points[face->vertices[4]].pos[2]);
        rlVertex3f(points[face->vertices[3]].pos[0], points[face->vertices[3]].pos[1],
                   points[face->vertices[3]].pos[2]);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        break;
    }
    rlEnd();
}

void Renderer_RenderCar(TerepCar* car)
{
    for (size_t i = 0; i < car->pointCount; i++) {
        RenderPoint(&car->points[i]);
    }
    for (size_t i = 0; i < car->physSegmentCount; i++) {
        RenderPhysicsSegment(&car->physSegments[i], car->points);
    }
    for (size_t i = 0; i < car->polygonCount; i++) {
        RenderPolygon(&car->polygons[i], car->points);
    }
}
