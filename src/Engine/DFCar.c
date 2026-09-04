#include "DFCar.h"
#include "Engine/Engine.h"
#include "LibTerep/TerepCar.h"
#include <raylib.h>
#include <rlgl.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void DFCar_RenderPoint(TerepCarPoint* point, Color tint)
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

void DFCar_RenderPhysicsLink(TerepCarPhysLink* seg, Color tint)
{
    DrawLine3D(ToVector3(seg->pointA->pos), ToVector3(seg->pointB->pos), tint);
}

void DFCar_RenderPolygonColored(TerepCarPolygonData* face, Color tint)
{
    if (face->colors[0] == 240)
        return;
    Color color = Engine_Palette[face->colors[0]];
    color = ColorTint(color, tint);
    if (face->vertices[0] == face->vertices[2]) {
        DrawLine3D(ToVector3(face->vertices[0]->pos), ToVector3(face->vertices[1]->pos), color);
        return;
    }
    rlBegin(RL_TRIANGLES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    switch (face->vertexCount) {
    case 3:
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);
        rlVertex3f(face->vertices[1]->pos[0], face->vertices[1]->pos[1], face->vertices[1]->pos[2]);
        rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);
        break;
    case 4:
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);
        rlVertex3f(face->vertices[1]->pos[0], face->vertices[1]->pos[1], face->vertices[1]->pos[2]);
        rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);

        rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);
        rlVertex3f(face->vertices[3]->pos[0], face->vertices[3]->pos[1], face->vertices[3]->pos[2]);
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);
        break;
    case 5:
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);
        rlVertex3f(face->vertices[1]->pos[0], face->vertices[1]->pos[1], face->vertices[1]->pos[2]);
        rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);

        rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);
        rlVertex3f(face->vertices[4]->pos[0], face->vertices[4]->pos[1], face->vertices[4]->pos[2]);
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);

        rlVertex3f(face->vertices[4]->pos[0], face->vertices[4]->pos[1], face->vertices[4]->pos[2]);
        rlVertex3f(face->vertices[3]->pos[0], face->vertices[3]->pos[1], face->vertices[3]->pos[2]);
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);
        break;
    }
    rlEnd();
}

void DFCar_RenderPolygonTextured(TerepCarPolygonData* face, Texture tex, Color tint)
{
    rlBegin(RL_TRIANGLES);
    rlSetTexture(tex.id);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);

    switch (face->vertexCount) {
    case 3:
        rlTexCoord2f(face->uv[2].x, face->uv[2].y);
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);
        rlTexCoord2f(face->uv[1].x, face->uv[1].y);
        rlVertex3f(face->vertices[1]->pos[0], face->vertices[1]->pos[1], face->vertices[1]->pos[2]);
        rlTexCoord2f(face->uv[0].x, face->uv[0].y);
        rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);
        break;
    case 4:
        rlTexCoord2f(face->uv[2].x, face->uv[2].y);
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);
        rlTexCoord2f(face->uv[1].x, face->uv[1].y);
        rlVertex3f(face->vertices[1]->pos[0], face->vertices[1]->pos[1], face->vertices[1]->pos[2]);
        rlTexCoord2f(face->uv[0].x, face->uv[0].y);
        rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);

        rlTexCoord2f(face->uv[0].x, face->uv[0].y);
        rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);
        rlTexCoord2f(face->uv[3].x, face->uv[3].y);
        rlVertex3f(face->vertices[3]->pos[0], face->vertices[3]->pos[1], face->vertices[3]->pos[2]);
        rlTexCoord2f(face->uv[2].x, face->uv[2].y);
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);
        break;
    case 5:
        rlTexCoord2f(face->uv[2].x, face->uv[2].y);
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);
        rlTexCoord2f(face->uv[1].x, face->uv[1].y);
        rlVertex3f(face->vertices[1]->pos[0], face->vertices[1]->pos[1], face->vertices[1]->pos[2]);
        rlTexCoord2f(face->uv[0].x, face->uv[0].y);
        rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);

        rlTexCoord2f(face->uv[0].x, face->uv[0].y);
        rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);
        rlTexCoord2f(face->uv[4].x, face->uv[4].y);
        rlVertex3f(face->vertices[4]->pos[0], face->vertices[4]->pos[1], face->vertices[4]->pos[2]);
        rlTexCoord2f(face->uv[2].x, face->uv[2].y);
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);

        rlTexCoord2f(face->uv[4].x, face->uv[4].y);
        rlVertex3f(face->vertices[4]->pos[0], face->vertices[4]->pos[1], face->vertices[4]->pos[2]);
        rlTexCoord2f(face->uv[3].x, face->uv[3].y);
        rlVertex3f(face->vertices[3]->pos[0], face->vertices[3]->pos[1], face->vertices[3]->pos[2]);
        rlTexCoord2f(face->uv[2].x, face->uv[2].y);
        rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);
        break;
    }
    rlEnd();
    rlSetTexture(rlGetTextureIdDefault());
}

void DFCar_Render(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    for (size_t i = 0; i < car->renderDataCount; i++) {
        if (car->renderData[i].type == TEREP_RENDERDATA_TEXTURE_POLYGON) {
            DFCar_RenderPolygonTextured(car->renderData[i].polygon, dfcar->carTex, WHITE);
        } else if (car->renderData[i].type == TEREP_RENDERDATA_COLOR_POLYGON && car->renderData[i].polygon->closed) {
            DFCar_RenderPolygonColored(car->renderData[i].polygon, WHITE);
        }
    }
}

DFCar* DFCar_Load()
{
    DFCar* dfcar = calloc(1, sizeof(DFCar));
    assert(dfcar);
    dfcar->car = TerepCar_Load("./data/car1.dat", "./data/car1.pcx");
    dfcar->carTex = LoadTextureFromImage((Image){
        .data = dfcar->car->carTexture->data,
        .height = dfcar->car->carTexture->height,
        .width = dfcar->car->carTexture->width,
        .format = 7,
        .mipmaps = 1,
    });
    strncpy(dfcar->name, "./data/car1x.dat", sizeof(dfcar->name));
    return dfcar;
}
void DFCar_Unload(DFCar* dfcar)
{
    TerepCar_Unload(dfcar->car);
    UnloadTexture(dfcar->carTex);
    free(dfcar);
}
