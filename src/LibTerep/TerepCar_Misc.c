#define LIBTEREP_INTERNAL
#include "TerepCar.h"
#include "LibTerep.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const char* TerepCar_Point2String(TerepCarPoint* point)
{
    switch (point->type) {
    case TEREP_POINT_GEOMETRY:
        return "GEOMETRY";
    case TEREP_POINT_CAMERA:
        return "CAMERA";
    case TEREP_POINT_WHEEL_FRONT:
        return "WHEEL_F";
    case TEREP_POINT_WHEEL_REAR:
        return "WHEEL_R";
    default:
        LTASSERT(0 && "Unknown point type");
    }
}

const char* TerepCar_PhysLink2String(TerepCarPhysLink* link)
{
    switch (link->type) {
    case TEREP_PHYSLINK_NORMAL:
        return "NORMAL";
    case TEREP_PHYSLINK_SUSP_EXTRA:
        return "SUSP_EXTRA";
    case TEREP_PHYSLINK_SUSP_REAR4:
        return "REAR4";
    case TEREP_PHYSLINK_SUSP_REAR6:
        return "REAR6";
    case TEREP_PHYSLINK_SUSP_FRONT10:
        return "FRONT10";
    case TEREP_PHYSLINK_SUSP_FRONT12:
        return "FRONT12";
    default:
        LTASSERT(0 && "Unknown link type");
    }
}

const char* TerepCar_RenderType2String(TerepCarRenderDataItem* item)
{
    switch (item->type) {
    case TEREP_RENDERDATA_NULL:
        return "NULL";
    case TEREP_RENDERDATA_CAMERA:
        return "CAMERADATA";
    case TEREP_RENDERDATA_UNK3_POLYGON:
        return "UNK3_POLYGON";
    case TEREP_RENDERDATA_COLOR_POLYGON:
        return "COLOR_POLYGON";
    case TEREP_RENDERDATA_TEXTURE_POLYGON:
        return "TEXTURE_POLYGON";
    case TEREP_RENDERDATA_WHEEL:
        return "WHEELDATA";
    default:
        LTASSERT(0 && "Unknown item type");
    }
}

void TerepCar_Unload(TerepCar* car)
{
    LTINFO("Unloading car...");
    if (car) {
        for (int i = 0; i < car->renderDataCount; i++) {
            switch (car->renderData[i].type) {
            case TEREP_RENDERDATA_NULL:
                break;
            case TEREP_RENDERDATA_CAMERA:
                free(car->renderData[i].camera);
                break;
            case TEREP_RENDERDATA_UNK3_POLYGON:
            case TEREP_RENDERDATA_COLOR_POLYGON:
            case TEREP_RENDERDATA_TEXTURE_POLYGON:
                free(car->renderData[i].polygon);
                break;
            case TEREP_RENDERDATA_WHEEL:
                free(car->renderData[i].wheel);
                break;
            }
        }
        free(car->renderData);
        free(car->points);
        free(car->physLinks);
        if (car->carTexture != 0) {
            free(car->carTexture->data);
            free(car->carTexture);
        }
        free(car);
    }
    printf("OK!\n");
}

static float _CalculateVector3Distance(float* v1, float* v2)
{
    float x = v2[0] - v1[0];
    float y = v2[1] - v1[1];
    float z = v2[2] - v1[2];
    float res = sqrtf(x*x + y*y + z*z);
    return res;
}

void TerepCar_RecalculatePhysLink(TerepCarPhysLink* link)
{
        link->len = _CalculateVector3Distance(link->pointA->pos, link->pointB->pos);
        link->len2 = link->len;
        if (link->type == TEREP_PHYSLINK_SUSP_FRONT10) {
            link->len_min = link->len * 0.48;
            link->len_max = link->len;
        } else if (link->type == TEREP_PHYSLINK_SUSP_FRONT12) {
            link->len_min = link->len * 0.48;
            link->len_max = link->len;
        } else if (link->type == TEREP_PHYSLINK_SUSP_REAR4) {
            link->len_min = link->len * 0.45;
            link->len_max = link->len;
        } else if (link->type == TEREP_PHYSLINK_SUSP_REAR6) {
            link->len_min = link->len * 0.45;
            link->len_max = link->len;
        } else if (link->type == TEREP_PHYSLINK_SUSP_EXTRA) {
            link->len_min = link->len * 0.45;
            link->len_max = link->len * 1.05;
        } else {
            link->len_min = link->len * 0.5;
            link->len_max = link->len * 1.5;
        }
}
void TerepCar_RecalculateAllPhysLinks(TerepCar* car)
{
    for (int j = 0; j < car->physLinkCount; j++) {
        TerepCarPhysLink* p = &car->physLinks[j];
        TerepCar_RecalculatePhysLink(p);
    }
}
