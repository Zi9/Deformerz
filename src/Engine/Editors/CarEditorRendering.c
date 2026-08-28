#include "CarEditor.h"
#include "Engine/Engine.h"
#include "Engine/Rendering/DFCamera.h"
#include "LibTerep/TerepCar.h"
#include "raymath.h"
#include <raylib.h>
#include <rlgl.h>

bool RenderPoints = true;
bool RenderPhysics = false;
bool RenderPolygons = true;

static inline Vector3 ToVector3(float v[3]) { return (Vector3){v[0], v[1], v[2]}; }

static void RenderPoint(TerepCarPoint* point, Color col)
{
    Vector3 pos = ToVector3(point->pos);
    DrawCube(pos, 0.02f, 0.02f, 0.02f, col);
    if (point->size > 0) {
        if (point->type == TEREP_POINT_CAMERA) {
            DrawSphereWires(pos, point->size, 8, 8, PINK);
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

static void RenderPolygonColored(TerepCarPolygonData* face, TerepCarPoint* points)
{
    if (face->colors[0] == 240)
        return;
    Color color = Engine.palette[face->colors[0]];
    if (face->vertices[0] == face->vertices[2]) {
        DrawLine3D(ToVector3(points[face->vertices[0]].pos), ToVector3(points[face->vertices[1]].pos), color);
        return;
    }
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, 128);
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
static void RenderPolygonTextured(TerepCarPolygonData* face, TerepCarPoint* points, Texture tex)
{
    Color color = WHITE;
    rlBegin(RL_TRIANGLES);
    rlSetTexture(tex.id);
    rlColor4ub(color.r, color.g, color.b, 128);

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
    rlSetTexture(rlGetTextureIdDefault());
}

void CarEditor_Render3D(DFCar* dfcar)
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
        case TEREP_POINT_WHEEL_REAR:
            col = BLUE;
            break;
        }
        if (SelectedPoint == -1) {
            Vector3 pos = ToVector3(car->points[i].pos);
            Ray ray = GetScreenToWorldRay(GetMousePosition(), DFCamera_GetRLCamera());
            RayCollision rc = GetRayCollisionBox(
                ray, (BoundingBox){.min = Vector3SubtractValue(pos, 0.01f), .max = Vector3AddValue(pos, 0.01f)});
            col.a = rc.hit ? 255 : 128;
            RenderPoint(&car->points[i], col);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && rc.hit) {
                SelectedPoint = i;
            }
        } else {
            if (i == SelectedPoint) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    SelectedPoint = -1;
                }
                RenderPoint(&car->points[i], GREEN);
                for (int j = 0; j < car->physSegmentCount; j++) {
                    if (car->physSegments[j].pointA == i || car->physSegments[j].pointB == i) {
                        RenderPhysicsSegment(&car->physSegments[j], car->points);
                    }
                }
            }
        }
    }
    if (RenderPolygons) {
        for (int i = 0; i < car->renderDataCount; i++) {
            if (car->renderData[i].type == TEREP_RENDERDATA_TEXTURE_POLYGON) {
                RenderPolygonTextured(car->renderData[i].polygon, car->points, dfcar->carTex);
            } else if (car->renderData[i].type == TEREP_RENDERDATA_COLOR_POLYGON &&
                       car->renderData[i].polygon->colors[0] != 0) {
                RenderPolygonColored(car->renderData[i].polygon, car->points);
            }
        }
    }
}
