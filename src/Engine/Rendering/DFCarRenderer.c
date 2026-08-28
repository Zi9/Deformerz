#include "LibTerep/TerepCar.h"
#include "Renderer.h"
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Render_CarPoint(TerepCarPoint* point, Color tint)
{
    Vector3 pos = ToVector3(point->pos);
    DrawCube(pos, 0.02f, 0.02f, 0.02f, tint);
    if (point->size > 0) {
        if (point->type == TEREP_POINT_CAMERA) {
            DrawSphereWires(pos, point->size, 8, 8, PINK);
        } else {
            DrawCircle3D(pos, point->size, (Vector3){0.0f, 1.0f, 0.0f}, 90, PINK);
        }
    }
}

void Render_CarPhysicsSegment(TerepCarPhysSegment* seg, TerepCarPoint* points, Color tint)
{
    DrawLine3D(ToVector3(points[seg->pointA].pos), ToVector3(points[seg->pointB].pos), tint);
}

void Render_CarPolygonColored(TerepCarPolygonData* face, TerepCarPoint* points, Color tint)
{
    if (face->colors[0] == 240)
        return;
    Color color = Renderer_Palette[face->colors[0]];
    color = ColorTint(color, tint);
    if (face->vertices[0] == face->vertices[2]) {
        DrawLine3D(ToVector3(points[face->vertices[0]].pos), ToVector3(points[face->vertices[1]].pos), color);
        return;
    }
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

void Render_CarPolygonTextured(TerepCarPolygonData* face, TerepCarPoint* points, Texture tex, Color tint)
{
    rlBegin(RL_TRIANGLES);
    rlSetTexture(tex.id);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

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

void Render_DFCar(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    for (size_t i = 0; i < car->renderDataCount; i++) {
        if (car->renderData[i].type == TEREP_RENDERDATA_TEXTURE_POLYGON) {
            Render_CarPolygonTextured(car->renderData[i].polygon, car->points, dfcar->carTex, WHITE);
        } else if (car->renderData[i].type == TEREP_RENDERDATA_COLOR_POLYGON &&
                   car->renderData[i].polygon->closed) {
            Render_CarPolygonColored(car->renderData[i].polygon, car->points, WHITE);
        }
    }
}
