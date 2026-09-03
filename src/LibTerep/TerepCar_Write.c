#define LIBTEREP_INTERNAL
#include "TerepCar.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static void _WriteChunk1(TerepCar* car, TerepDat* dat)
{
    I16(dat->data, 0) = dat->cur - dat->data;
    U16(dat->cur, 0) = car->pointCount;
    dat->cur += 2;
    for (size_t i = 0; i < car->pointCount; i++) {
        I16(dat->cur, 0) = car->points[i].unknown[0];
        I16(dat->cur, 2) = car->points[i].pos[0] * SCALE;
        I16(dat->cur, 8) = car->points[i].unknown[1];
        I16(dat->cur, 10) = car->points[i].pos[1] * SCALE;
        I16(dat->cur, 4) = car->points[i].unknown[2];
        I16(dat->cur, 6) = car->points[i].pos[2] * SCALE;
        I16(dat->cur, 24) = car->points[i].size * SCALE;
        I16(dat->cur, 26) = car->points[i].type;
        dat->cur += 28;
    }
    printf("LibTerep | INFO: Built %d points for %s\n", car->pointCount, dat->name);
}
static void _WriteChunk2(TerepCar* car, TerepDat* dat)
{
    I16(dat->data, 2) = dat->cur - dat->data;
    U16(dat->cur, 0) = car->physLinkCount;
    dat->cur += 2;
    for (size_t i = 0; i < car->physLinkCount; i++) {
        U16(dat->cur, 0) = car->physLinks[i].pointA->index;
        U16(dat->cur, 2) = car->physLinks[i].pointB->index;
        U16(dat->cur, 4) = round(car->physLinks[i].len * PHYS_LINK_SCALE);
        U16(dat->cur, 6) = round(car->physLinks[i].len2 * PHYS_LINK_SCALE);
        U16(dat->cur, 8) = car->physLinks[i].type;
        U16(dat->cur, 10) = round(car->physLinks[i].len_min * PHYS_LINK_SCALE);
        U16(dat->cur, 12) = round(car->physLinks[i].len_max * PHYS_LINK_SCALE);
        dat->cur += 14;
    }
    printf("LibTerep | INFO: Built %d physics links for %s\n", car->physLinkCount, dat->name);
}

static void _WriteChunk3(TerepCar* car, TerepDat* dat)
{
    I16(dat->data, 4) = dat->cur - dat->data;
    for (int i = 0; i < car->renderDataCount; i++) {
        U8(dat->cur, 0) = car->renderData[i].type;
        dat->cur++;
        switch (car->renderData[i].type) {
        case TEREP_RENDERDATA_NULL:
            break;
        case TEREP_RENDERDATA_CAMERA: {
            TerepCarCameraData* cam = car->renderData[i].camera;
            U16(dat->cur, 0) = cam->cameraPoint->index * 2;
            U8(dat->cur, 2) = cam->unknown1;
            U8(dat->cur, 3) = cam->unknown2;
            dat->cur += 4;
            break;
        }
        case TEREP_RENDERDATA_UNK3_POLYGON: {
            // possibly some culling thing, changing these values seems to do render glitches
            TerepCarPolygonData* polygon = car->renderData[i].polygon;
            U16(dat->cur, 0) = polygon->vertices[0]->index * 2;
            U16(dat->cur, 2) = polygon->vertices[1]->index * 2;
            U16(dat->cur, 4) = polygon->vertices[2]->index * 2;
            U16(dat->cur, 6) = polygon->unknown3values[0];
            U16(dat->cur, 8) = polygon->unknown3values[1];
            U16(dat->cur, 10) = polygon->unknown3values[2];
            dat->cur += 12;
            break;
        }
        case TEREP_RENDERDATA_COLOR_POLYGON: {
            TerepCarPolygonData* polygon = car->renderData[i].polygon;
            U8(dat->cur, 0) = polygon->vertexCount;
            dat->cur++;
            for (size_t i = 0; i < polygon->vertexCount; i++) {
                if (!polygon->closed) {
                    U16(dat->cur, 0) = polygon->vertices[i]->index * 2 + 1;
                } else {
                    U16(dat->cur, 0) = polygon->vertices[i]->index * 2;
                }
                dat->cur += 2;
            }
            if (polygon->closed) {
                U16(dat->cur, 0) = polygon->vertices[0]->index * 2;
            } else {

                U16(dat->cur, 0) = 0;
            }
            dat->cur += 2;
            U8(dat->cur, 0) = polygon->colors[0];
            U8(dat->cur, 1) = polygon->colors[1]; // NOTE: Dithering
            dat->cur += 2;
            break;
        }
        case TEREP_RENDERDATA_TEXTURE_POLYGON: {
            TerepCarPolygonData* polygon = car->renderData[i].polygon;
            U8(dat->cur, 0) = polygon->vertexCount;
            dat->cur++;
            for (size_t i = 0; i < polygon->vertexCount; i++) {
                U16(dat->cur, 0) = polygon->vertices[i]->index * 2;
                U16(dat->cur, 2) = polygon->uv[i].x;
                U16(dat->cur, 4) = polygon->uv[i].y;
                dat->cur += 3 * 2;
            }
            if (polygon->closed) {
                U16(dat->cur, 0) = polygon->vertices[0]->index * 2;
                U16(dat->cur, 2) = polygon->uv[0].x;
                U16(dat->cur, 4) = polygon->uv[0].y;
            } else {
                U16(dat->cur, 0) = 0;
                U16(dat->cur, 2) = 0;
                U16(dat->cur, 4) = 0;
            }
            dat->cur += 3 * 2;
            break;
        }
        case TEREP_RENDERDATA_WHEEL: {
            TerepCarWheelData* wheel = car->renderData[i].wheel;
            U16(dat->cur, 0) = wheel->wheelPoint->index * 2;
            U16(dat->cur, 2) = wheel->unknown1;
            U16(dat->cur, 4) = wheel->unknown2;
            dat->cur += 3 * 2;
            for (int i = 0; i < 9; i++) {
                U16(dat->cur, 0) = wheel->wheelSprites[i].sz_height;
                U16(dat->cur, 2) = wheel->wheelSprites[i].sz_width;
                dat->cur += 4;
                for (int j = 0; j < 4; j++) {
                    U16(dat->cur, 0) = round(wheel->wheelSprites[i].UV[j].x * 65535.0f);
                    U16(dat->cur, 2) = round(wheel->wheelSprites[i].UV[j].y * 65535.0f);
                    dat->cur += 4;
                }
            }
            break;
        }
        }
    }
    printf("LibTerep | INFO: Built %i items for %s\n", car->renderDataCount, dat->name);
}
void TerepCar_Write(TerepCar* car, const char* cardat, const char* carpcx)
{
    TerepDat* dat = _CreateDat();
    strncpy(dat->name, cardat, 32);

    dat->cur = dat->data + 132;
    _WriteChunk1(car, dat);
    _WriteChunk2(car, dat);
    _WriteChunk3(car, dat);
    U16(dat->data, 6) = car->unknownHeaderValue1;
    U16(dat->data, 8) = car->engineSound;
    dat->size = dat->cur - dat->data;

    FILE* fp = fopen(cardat, "wb");
    printf("LibTerep | INFO: Attempting to write %zu bytes to %s...\n", dat->size, dat->name);
    fwrite(dat->data, dat->size, 1, fp);
    fclose(fp);
    printf("LibTerep | INFO: Finished writing %s!\n", dat->name);
    _UnloadDat(dat);
}
