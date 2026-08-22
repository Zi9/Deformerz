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

static void RenderPolygonColored(TerepCarPolygon* face, TerepCarPoint* points)
{
    if (face->colors[0] == 240) return;
    Color color = Engine.palette[face->colors[0]];
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    switch (face->pointCount) {
    case 3:
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        rlVertex3f(points[face->vertices[1]].pos[0], points[face->vertices[1]].pos[1],
                   points[face->vertices[1]].pos[2]);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        break;
    case 4:
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        rlVertex3f(points[face->vertices[1]].pos[0], points[face->vertices[1]].pos[1],
                   points[face->vertices[1]].pos[2]);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);

        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        rlVertex3f(points[face->vertices[3]].pos[0], points[face->vertices[3]].pos[1],
                   points[face->vertices[3]].pos[2]);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        break;
    case 5:
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        rlVertex3f(points[face->vertices[1]].pos[0], points[face->vertices[1]].pos[1],
                   points[face->vertices[1]].pos[2]);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        rlVertex3f(points[face->vertices[4]].pos[0], points[face->vertices[4]].pos[1],
                   points[face->vertices[4]].pos[2]);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
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

static void RenderPolygonTextured(TerepCarPolygon* face, TerepCarPoint* points, Texture tex)
{
    Color color = WHITE;
    rlSetTexture(tex.id);
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    switch (face->pointCount) {
    case 3:
        rlTexCoord2f(face->uv[2].x / 65535.0, face->uv[2].y / 65535.0);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        rlTexCoord2f(face->uv[1].x / 65535.0, face->uv[1].y / 65535.0);
        rlVertex3f(points[face->vertices[1]].pos[0], points[face->vertices[1]].pos[1],
                   points[face->vertices[1]].pos[2]);
        rlTexCoord2f(face->uv[0].x / 65535.0, face->uv[0].y / 65535.0);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        break;
    case 4:
        rlTexCoord2f(face->uv[2].x / 65535.0, face->uv[2].y / 65535.0);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        rlTexCoord2f(face->uv[1].x / 65535.0, face->uv[1].y / 65535.0);
        rlVertex3f(points[face->vertices[1]].pos[0], points[face->vertices[1]].pos[1],
                   points[face->vertices[1]].pos[2]);
        rlTexCoord2f(face->uv[0].x / 65535.0, face->uv[0].y / 65535.0);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);

        rlTexCoord2f(face->uv[0].x / 65535.0, face->uv[0].y / 65535.0);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        rlTexCoord2f(face->uv[3].x / 65535.0, face->uv[3].y / 65535.0);
        rlVertex3f(points[face->vertices[3]].pos[0], points[face->vertices[3]].pos[1],
                   points[face->vertices[3]].pos[2]);
        rlTexCoord2f(face->uv[2].x / 65535.0, face->uv[2].y / 65535.0);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        break;
    case 5:
        rlTexCoord2f(face->uv[2].x / 65535.0, face->uv[2].y / 65535.0);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        rlTexCoord2f(face->uv[1].x / 65535.0, face->uv[1].y / 65535.0);
        rlVertex3f(points[face->vertices[1]].pos[0], points[face->vertices[1]].pos[1],
                   points[face->vertices[1]].pos[2]);
        rlTexCoord2f(face->uv[0].x / 65535.0, face->uv[0].y / 65535.0);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);

        rlTexCoord2f(face->uv[0].x / 65535.0, face->uv[0].y / 65535.0);
        rlVertex3f(points[face->vertices[0]].pos[0], points[face->vertices[0]].pos[1],
                   points[face->vertices[0]].pos[2]);
        rlTexCoord2f(face->uv[4].x / 65535.0, face->uv[4].y / 65535.0);
        rlVertex3f(points[face->vertices[4]].pos[0], points[face->vertices[4]].pos[1],
                   points[face->vertices[4]].pos[2]);
        rlTexCoord2f(face->uv[2].x / 65535.0, face->uv[2].y / 65535.0);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);

        rlTexCoord2f(face->uv[4].x / 65535.0, face->uv[4].y / 65535.0);
        rlVertex3f(points[face->vertices[4]].pos[0], points[face->vertices[4]].pos[1],
                   points[face->vertices[4]].pos[2]);
        rlTexCoord2f(face->uv[3].x / 65535.0, face->uv[3].y / 65535.0);
        rlVertex3f(points[face->vertices[3]].pos[0], points[face->vertices[3]].pos[1],
                   points[face->vertices[3]].pos[2]);
        rlTexCoord2f(face->uv[2].x / 65535.0, face->uv[2].y / 65535.0);
        rlVertex3f(points[face->vertices[2]].pos[0], points[face->vertices[2]].pos[1],
                   points[face->vertices[2]].pos[2]);
        break;
    }
    rlEnd();
    rlSetTexture(0);
}

void Renderer_RenderCar(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    for (size_t i = 0; i < car->pointCount; i++) {
        if (car->points[i].type == TEREP_POINT_GEOMETRY) continue;
        RenderPoint(&car->points[i]);
    }
    for (size_t i = 0; i < car->physSegmentCount; i++) {
        if (car->physSegments[i].type == TEREP_SEGMENT_NORMAL) continue;
        RenderPhysicsSegment(&car->physSegments[i], car->points);
    }
    for (size_t i = 0; i < car->polygonCount; i++) {
        if (car->polygons[i].type == TEREP_POLYGON_TEXTURE) {
            RenderPolygonTextured(&car->polygons[i], car->points, dfcar->carTex);
        }
        else if (car->polygons[i].type == TEREP_POLYGON_COLOR && car->polygons[i].colors[0] != 0) {
            RenderPolygonColored(&car->polygons[i], car->points);
        }
    }
}
