#define LIBTEREP_INTERNAL
#include "TerepCar.h"

#include <stdlib.h>
#include <stdio.h>

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
    }
}

const char* TerepCar_PhysLink2String(TerepCarPhysLink* seg)
{
    switch (seg->type) {
    case TEREP_PHYSLINK_NORMAL:
        return "NORMAL";
    case TEREP_PHYSLINK_SUSP_EXTRA:
        return "SUSP_EXTRA";
    case TEREP_PHYSLINK_SUSP_FRONT:
        return "FRONT";
    case TEREP_PHYSLINK_SUSP_REAR:
        return "REAR";
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
    }
}

void TerepCar_Unload(TerepCar* car)
{
    printf("LibTerep | INFO: Unloading car...");
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
    printf("OK!\n");
}
