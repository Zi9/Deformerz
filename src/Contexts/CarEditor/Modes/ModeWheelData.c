#include "Contexts/CarEditor/CarEditor.h"
#include "Engine/DFCar.h"
#include "Engine/UI.h"
#include "LibTerep/TerepCar.h"
#include "cimgui.h"
#include "raylib.h"
#include <rlgl.h>

static int idx = 0;

static void _3D(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    for (int i = 0; i < car->pointCount; i++) {
        switch (car->points[i].type) {
        case TEREP_POINT_GEOMETRY:
            break;
        case TEREP_POINT_CAMERA:
            DFCar_RenderPoint(&car->points[i], MAGENTA);
            break;
        case TEREP_POINT_WHEEL_FRONT:
            DFCar_RenderPoint(&car->points[i], RED);
            break;
        case TEREP_POINT_WHEEL_REAR:
            DFCar_RenderPoint(&car->points[i], BLUE);
            break;
        }
    }
    for (int i = 0; i < car->renderDataCount; i++) {
        if (car->renderData[i].type == TEREP_RENDERDATA_TEXTURE_POLYGON) {
            DFCar_RenderPolygonTextured(car->renderData[i].polygon, dfcar->carTex, ColorAlpha(WHITE, 0.5f));
        } else if (car->renderData[i].type == TEREP_RENDERDATA_COLOR_POLYGON && car->renderData[i].polygon->closed) {
            DFCar_RenderPolygonColored(car->renderData[i].polygon, ColorAlpha(WHITE, 0.5f));
        } else if (car->renderData[i].type == TEREP_RENDERDATA_WHEEL) {
            TerepCarWheelData* wheel = car->renderData[i].wheel;
            rlBegin(RL_TRIANGLES);
            rlSetTexture(dfcar->carTex.id);
            rlColor4ub(255, 255, 255, 255);

            Vector3 pos = ToVector3(wheel->wheelPoint->pos);
            float szx = wheel->wheelSprites[idx].sz_width / 65535.0;
            float szy = wheel->wheelSprites[idx].sz_height / 65535.0f;

            rlTexCoord2f(wheel->wheelSprites[idx].UV[3].x, wheel->wheelSprites[idx].UV[3].y);
            rlVertex3f(pos.x, pos.y - szy, pos.z - szx);
            rlTexCoord2f(wheel->wheelSprites[idx].UV[2].x, wheel->wheelSprites[idx].UV[2].y);
            rlVertex3f(pos.x, pos.y - szy, pos.z + szx);
            rlTexCoord2f(wheel->wheelSprites[idx].UV[0].x, wheel->wheelSprites[idx].UV[0].y);
            rlVertex3f(pos.x, pos.y + szy, pos.z - szx);

            rlTexCoord2f(wheel->wheelSprites[idx].UV[2].x, wheel->wheelSprites[idx].UV[2].y);
            rlVertex3f(pos.x, pos.y - szy, pos.z + szx);
            rlTexCoord2f(wheel->wheelSprites[idx].UV[1].x, wheel->wheelSprites[idx].UV[1].y);
            rlVertex3f(pos.x, pos.y + szy, pos.z + szx);
            rlTexCoord2f(wheel->wheelSprites[idx].UV[0].x, wheel->wheelSprites[idx].UV[0].y);
            rlVertex3f(pos.x, pos.y + szy, pos.z - szx);
            // rlTexCoord2f(face->uv[1].x / 65535.0, face->uv[1].y / 65535.0);
            // rlVertex3f(face->vertices[1]->pos[0], face->vertices[1]->pos[1], face->vertices[1]->pos[2]);
            // rlTexCoord2f(face->uv[0].x / 65535.0, face->uv[0].y / 65535.0);
            // rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);

            // rlTexCoord2f(face->uv[0].x / 65535.0, face->uv[0].y / 65535.0);
            // rlVertex3f(face->vertices[0]->pos[0], face->vertices[0]->pos[1], face->vertices[0]->pos[2]);
            // rlTexCoord2f(face->uv[3].x / 65535.0, face->uv[3].y / 65535.0);
            // rlVertex3f(face->vertices[3]->pos[0], face->vertices[3]->pos[1], face->vertices[3]->pos[2]);
            // rlTexCoord2f(face->uv[2].x / 65535.0, face->uv[2].y / 65535.0);
            // rlVertex3f(face->vertices[2]->pos[0], face->vertices[2]->pos[1], face->vertices[2]->pos[2]);
            rlEnd();
            rlSetTexture(rlGetTextureIdDefault());
        }
    }
}

static void _UI(DFCar* dfcar)
{
    TerepCar* car = dfcar->car;
    if (igButton("<- Back", (ImVec2){0})) {
        CarEditor_SwitchMode(CarEditorMode_Meta());
    }
    igSameLine(0, 8);
    igText("Wheel Data");
    igSeparator();

    igInputInt("Sprite IDX", &idx, 1, 1, 0);
    if (idx == 9)
        idx = 0;

    for (int i = 0; i < car->renderDataCount; i++) {
        if (car->renderData[i].type != TEREP_RENDERDATA_WHEEL)
            continue;
        TerepCarWheelData* wheel = car->renderData[i].wheel;
        igPushID_Int(i);
        if (igTreeNode_StrStr("##", "Wheel %i", wheel->wheelPoint->index)) {
            igText("Unknowns %i, %i", wheel->unknown1, wheel->unknown2);
            igSeparatorText("Sprites");
            for (int j = 0; j < 9; j++) {
                igText("Wheel sprite %i - Height: %i, Width: %i", j, wheel->wheelSprites[j].sz_height,
                       wheel->wheelSprites[j].sz_width);
            }
            igTreePop();
        }
        igPopID();
    }
}

CarEditorMode CarEditorMode_WheelData() { return (CarEditorMode){.RenderUI = _UI, .Render3D = _3D}; }
