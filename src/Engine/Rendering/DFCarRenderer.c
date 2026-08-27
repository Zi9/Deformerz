#include "Engine/Engine.h"
#include "LibTerep/TerepCar.h"
#include <raylib.h>
#include <rlgl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void RenderPolygonColored(TerepCarPolygonData* face, TerepCarPoint* points)
{
    if (face->colors[0] == 240)
        return;
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

static void RenderPolygonTextured(TerepCarPolygonData* face, TerepCarPoint* points, Texture tex)
{
    Color color = WHITE;
    rlBegin(RL_TRIANGLES);
    rlSetTexture(tex.id);
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
    rlSetTexture(rlGetTextureIdDefault());
}

void Renderer_RenderCar(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    for (size_t i = 0; i < car->renderDataCount; i++) {
        if (car->renderData[i].type == TEREP_RENDERDATA_TEXTURE_POLYGON) {
            RenderPolygonTextured(car->renderData[i].polygon, car->points, dfcar->carTex);
        } else if (car->renderData[i].type == TEREP_RENDERDATA_COLOR_POLYGON && car->renderData[i].polygon->colors[0] != 0) {
            RenderPolygonColored(car->renderData[i].polygon, car->points);
        }
    }
}
